DiscordHook64 has a valid digital signature from Discord Inc. sigthief is used to steal/copy that signure to put it on OutfledOverlay.dll and OutfledOverlay64.dll. DiscordHook64 is a 64-bit dll
but can be used for both the 32 & 64 bit of OutfledOverlay.

Discord obviously has its own game overlay which is compatible with most games, which is why I chose to steal a Discord Inc. signature
for this. However, you can expirement using different files with trusted signatures.

The command used to steal & apply the signature onto the OutfledOverlay64.dll:

    sigthief.py -i DiscordHook64.dll -t ..\Build\OutfledOverlay64.dll -o ..\Build\OutfledOverlay64.dll

The command used to steal & apply the signature onto the OutfledOverlay.dll:

    sigthief.py -i DiscordHook64.dll -t ..\Build\OutfledOverlay.dll -o ..\Build\OutfledOverlay.dll

# Sigthief original repository:

https://github.com/secretsquirrel/SigThief
    
