// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CAlpacaCore

public enum DictConvertError: Error, Equatable {
    case invalidType(String)
}

public func translateDictionaryToDict(_ dictionary: Dictionary<String, Any>) throws -> AC.DictRoot {
    // Create the root dictionary object
    // var dictRoot = AC.DictRoot.create()
    let dictRoot = AC.DictRoot()

    func convertValue(sourceValue: Any, target: inout AC.DictRef) throws {
        if let intValue = sourceValue as? Int {
            target.setInt(intValue)
        } else if let unsignedValue = sourceValue as? UInt {
            target.setUnsigned(Int(unsignedValue))
        } else if let boolValue = sourceValue as? Bool {
            target.setBool(boolValue)
        } else if let doubleValue = sourceValue as? Double {
            target.setDouble(doubleValue)
        } else if let stringValue = sourceValue as? String {
            target.setString(std.string(stringValue))
        } else if let dataValue = sourceValue as? Data {
            dataValue.withUnsafeBytes { (ptr: UnsafeRawBufferPointer) in
                target.setBinary(ptr.baseAddress!, dataValue.count)
            }
        } else if let dictValue = sourceValue as? Dictionary<String, Any> {
            try convertDictionary(dictValue, into: target)
        } else if let arrayValue = sourceValue as? [Any] {
            for element in arrayValue {
                var arrayElement = target.addElement()
                try convertValue(sourceValue: element, target: &arrayElement)
            }
        } else {
            throw DictConvertError.invalidType("Invalid type (\(type(of: sourceValue))) for dictionary value")
        }
    }

    // Recursive function to convert the dictionary
    func convertDictionary(_ sourceDict: Dictionary<String, Any>, into targetDict: AC.DictRef) throws {
        for (key, value) in sourceDict {
            var child = targetDict.addChild(std.string(key))
            try convertValue(sourceValue: value, target: &child)
        }
    }

    // Convert the top-level dictionary
    try convertDictionary(dictionary, into: dictRoot.getRef())

    return dictRoot
}

public func translateDictToDictionary(_ dict: AC.DictRef) throws -> Dictionary<String, Any> {
    var dictionary: Dictionary<String, Any> = Dictionary<String, Any>()

    func convertValue(sourceValue: AC.DictRef) throws -> Any {
        switch sourceValue.getType() {
        case .Bool:
            return sourceValue.getBool()
        case .Int:
            return sourceValue.getInt()
        case .Unsigned:
            return sourceValue.getUnsigned()
        case .Double:
            return sourceValue.getDouble()
        case .String:
            return String(sourceValue.getString())
        case .Binary:
            let binaryData = sourceValue.getBinary()
            return Data(bytes: binaryData.data, count: binaryData.size)
        case .Array:
            var array: [Any] = []
            for index in 0..<Int32(sourceValue.getSize()) {
                array.append(try convertValue(sourceValue:sourceValue.atIndex(index)))
            }
            return array
        case .Object:
            var nestedDictionary: Dictionary<String, Any> = Dictionary<String, Any>()
            try convertDictRefToDictionary(sourceValue, into: &nestedDictionary)
            return nestedDictionary
        default:
        // case .Null:
            throw DictConvertError.invalidType("Invalid type for dictionary value")
        }
    }

    // Recursive function to convert DictRef to Dictionary
    func convertDictRefToDictionary(_ source: AC.DictRef, into target: inout Dictionary<String, Any>) throws {
        // Additionally, handle dictionary keys and their values
        for keyStr in source.getKeys() { // Assuming getDict() returns a dictionary-like object
            let child = source.atKey(keyStr)
            let key = String(keyStr)
            try target[key] = convertValue(sourceValue: child)
        }
    }

    // Start converting the DictRef
    try convertDictRefToDictionary(dict, into: &dictionary)

    return dictionary
}
