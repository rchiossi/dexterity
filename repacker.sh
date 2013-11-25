#!/bin/bash

# --- DEX Repacker v1 ---

#Generate Key
# keytool -genkey -v -keystore dexterity.keystore -alias dexterity -keyalg RSA -keysize 2048 -validity 10000

[ $# -eq 4 ] || { echo -e "Usage:\n\t$0 apk dex keystore alias"; exit 1; }

for i in zip unzip jarsigner; do
	which $i >/dev/null || { echo $i not found; exit 1; }
done

apk=$(basename $1)
dex=$(basename $2)
keystore=$(basename $3)
keyalias=$4

apkdir=$(cd $(dirname $1); pwd)
dexdir=$(cd $(dirname $2); pwd)
keydir=$(cd $(dirname $3); pwd)

echo -e \
"<repacker> --- DEX Repacker v1 ---\n\
<repacker> APK: $apk\n\
<repacker> DEX: $dex"

tmpfolder=$(mktemp -d)
target="${apk%.*}-repack.apk"

echo "<repacker> Extracting DEX file"
unzip -d $tmpfolder "$apkdir/$apk"

echo "<repacker> Replacing DEX"
cp "$dexdir/$dex" "$tmpfolder/classes.dex"

echo "<repacker> Removing meta info"
rm -rf "$tmpfolder/META-INF"

echo "<repacker> Repackaging APK"
rm -f $target
zip -r $target $tmpfolder

echo "<repacker> Signing the APK"
jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore "$keydir/$keystore" $target $keyalias

echo "<repacker> Cleaning tmp"
rm -rf $tmpfolder

echo "<repacker> Done."    
