# iOS asset management

This document will cover the possible solutions for the shared asset management specificly for iOS.

## Requirements

Since mobile platforms have different application architecture than desktop ones we need to check how we can use the already integrated download managers to be more inte

```
We'll think in a context of multiple apps, but all tests are done with 2 apps
```

- Use the integrated download mechanisms
    - Download a model through one app
    - Download a model one after the other
    - Download same model in the same time - is it possible?
- Share files with multiple apps - same models can be used within multiple apps
    - Store them locally on the FS
    - Where can we share common files?
    - Can both of them read the shared files?
        - What permissions are needed?
- Guard against malicious apps
    - Check hash on load of models?

## Asset downloading

TBD

## Possible approaches for model sharing

Due to the [sandbox architecture](https://developer.apple.com/documentation/security/app-sandbox) of the app we have limited access to the filesystem. We cannot write any file outside the app without any permission. That's why I'll mention all of the approaches I've found and the reason why most of them won't work for our use case.

### App Groups

At first this looked as the most suitable solution for us.

We can imagine it as a `string -> folder` pair. When a developer want to use a specific app group he just can add it as a capability of the app and get access to the shared folder.

Everything looks perfect, until we see the limitations of the `App Groups` - same developer. To use this feature the development team should be the same across the apps which is a blocker for us since we won't publish them and every developer who will use our SDK will have different development team.

### External cloud providers

Since one of the requirements is to store assets locally in order to load them faster we cannot use any cloud providers such as Amazon S3, iCloud, Firebase.

### URL schemes

Another approach is to add a capability in the property list for the app to define a url scheme - looks like this:

```xml
<key>CFBundleURLTypes</key>
<array>
    <dict>
        <key>CFBundleURLName</key>
        <string>com.yourcompany.customurlscheme</string>
        <key>CFBundleURLSchemes</key>
        <array>
            <string>yourappscheme</string>
        </array>
    </dict>
</array>

```

However, the requirement of the same development team persist here too.

Moreover, even we are same development team we have to create a scheme for each app and there is no shared file location. We can see it's not designed to be used for this kind of file sharing.

Just a small clarification here about the universal links since they are sometimes mistaken with the URL schemes for inter-app communication.

The [universal links](https://developer.apple.com/documentation/xcode/supporting-universal-links-in-your-app) for iOS is used to launch a specific app when it's being activated. During app developement you can add this capability to the app and choose which URL you'll support and then add a handling code. When being used developers must be careful since they might work as a potential attack.

### User provided permission for writing

This solution seems to be the only working properly for us. It has its disadvantage that it'll require input from the user to choose a shared folder where we can store the downloaded assets.

The steps for giving access to a folder are:

- Use our implementatation of `UIDocumentPickerDelegate` and let user to choose a folder
- Save the returned URL as a bookmark in the app's content folder, so we can restore it next time when the app is opened (so we don't need to ask the user again)
- Write/Read files from this shared folder

This might be error prompt from the user POV since he might choose different folder each time and then we won't share any models actually but since we don't have other viable solution we can stick with it for now.
