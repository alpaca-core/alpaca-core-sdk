// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CAlpacaCore

public func translateDictionaryToDict(_ dictionary: Dictionary<String, Any>) -> ac.swift.DictRoot {
    // Create the root dictionary object
    let dictRoot = ac.swift.DictRoot.create()

    // Recursive function to convert the dictionary
    func convertDictionary(_ sourceDict: Dictionary<String, Any>, into targetDict: ac.swift.DictRef) {
        for (key, value) in sourceDict {
            var child = targetDict.addChild(std.string(key))

            if let intValue = value as? Int {
                child.setInt(intValue)
            } else if let unsignedValue = value as? UInt {
                child.setUnsigned(Int(unsignedValue))
            } else if let boolValue = value as? Bool {
                child.setBool(boolValue)
            } else if let doubleValue = value as? Double {
                child.setDouble(doubleValue)
            } else if let stringValue = value as? String {
                child.setString(std.string(stringValue))
            } else if let dataValue = value as? Data {
                dataValue.withUnsafeBytes { (ptr: UnsafeRawBufferPointer) in
                    child.setBinary(ptr.baseAddress!, dataValue.count)
                }
            } else if let dictValue = value as? Dictionary<String, Any> {
                // If it's a nested dictionary, recursively convert
                convertDictionary(dictValue, into: child)
            } else if let arrayValue = value as? [Any] {
                // Handle array elements
                for element in arrayValue {
                    var arrayElement = child.addElement()
                    if let intElement = element as? Int {
                        arrayElement.setInt(intElement)
                    } else if let unsignedValue = value as? UInt {
                        arrayElement.setUnsigned(Int(unsignedValue))
                    } else if let boolElement = element as? Bool {
                        arrayElement.setBool(boolElement)
                    } else if let doubleElement = element as? Double {
                        arrayElement.setDouble(doubleElement)
                    } else if let stringElement = element as? String {
                        arrayElement.setString(std.string(stringElement))
                    } else if let dataValue = element as? Data {
                        dataValue.withUnsafeBytes { (ptr: UnsafeRawBufferPointer) in
                            arrayElement.setBinary(ptr.baseAddress!, dataValue.count)
                        }
                    } else if let dictElement = element as? Dictionary<String, Any> {
                        // Handle nested dictionaries inside the array
                        convertDictionary(dictElement, into: arrayElement)
                    }
                }
            } else {
                print("Unhandled type for key: \(key)")
            }
        }
    }

    // Convert the top-level dictionary
    convertDictionary(dictionary, into: dictRoot.getRef())

    return dictRoot
}

public func translateDictToDictionary(_ dict: ac.swift.DictRef) -> Dictionary<String, Any> {
    var dictionary: Dictionary<String, Any> = Dictionary<String, Any>()

    // Recursive function to convert DictRef to Dictionary
    func convertDictRefToDictionary(_ source: ac.swift.DictRef, into target: inout Dictionary<String, Any>) {
        // Additionally, handle dictionary keys and their values
        for keyStr in source.getKeys() { // Assuming getDict() returns a dictionary-like object
            let child = source.atKey(keyStr)
            let childType = child.getType()
            let key = String(keyStr)

            switch childType {
            case .Bool:
                target[key] = child.getBool()
            case .Int:
                target[key] = child.getInt()
            case .Unsigned:
                target[key] = child.getUnsigned()
            case .Double:
                target[key] = child.getDouble()
            case .String:
                target[key] = String(child.getString())
            case .Binary:
                let binaryData = child.getBinary()
                target[key] = Data(bytes: binaryData.data, count: binaryData.size)
            case .Array:
                var array: [Any] = []
                for index in 0..<Int32(child.getSize()) {
                    let valueType = child.atIndex(index).getType()
                    let value = child.atIndex(index)

                    switch valueType {
                    case .Bool:
                        array.append(value.getBool())
                    case .Int:
                        array.append(value.getInt())
                    case .Unsigned:
                        array.append(value.getUnsigned())
                    case .Double:
                        array.append(value.getDouble())
                    case .String:
                        array.append(String(value.getString()))
                    case .Binary:
                        let binaryData = value.getBinary()
                        array.append(Data(bytes: binaryData.data, count: binaryData.size))
                    case .Object:
                        // Handle nested object (DictRef)
                        var nestedDictionary: [String: Any] = [:]
                        convertDictRefToDictionary(value, into: &nestedDictionary)
                        array.append(nestedDictionary)
                    default:
                        break // Handle other types or ignore
                    }
                }
                target[key] = array
            case .Object:
                var nestedDictionary: Dictionary<String, Any> = Dictionary<String, Any>()
                convertDictRefToDictionary(child, into: &nestedDictionary)
                target[key] = nestedDictionary
            default:
                break // Handle other types or ignore
            }
        }
    }

    // Start converting the DictRef
    convertDictRefToDictionary(dict, into: &dictionary)

    return dictionary
}
