#!/bin/bash

# --- DEX Repacker v1 ---

#Generate Key
# keytool -genkey -v -keystore dexterity.keystore -alias dexterity -keyalg RSA -keysize 2048 -validity 10000

if  [ $# -ne 4 ]; then
    echo "$0 apk dex keystore alias"
else
    apk=$(basename $1)
    dex=$(basename $2)
    keystore=$(basename $3)
    keyalias=$4

    apkdir=$(cd $(dirname $1); pwd)
    dexdir=$(cd $(dirname $2); pwd)
    keydir=$(cd $(dirname $3); pwd)
   
    echo "<repacker> --- DEX Repacker v1 ---"
    echo "<repacker> APK: $apk"
    echo "<repacker> DEX: $dex"

    name=${apk%.*}
    
    tmpfolder="$name-tmp"
    target="$name-repack.apk"

    echo "<repacker> Extracting DEX file"

    if [ -d "$tmpfolder" ]; then
	rm -rf $tmpfolder
    fi

    unzip -d $tmpfolder "$apkdir/$apk"

    echo "<repacker> Replacing DEX"
    cp "$dexdir/$dex" "$tmpfolder/classes.dex"
    
    echo "<repacker> Removing meta info"
    rm -rf "$tmpfolder/META-INF"

    echo "<repacker> Repackaging APK"
    cd $tmpfolder; 
    zip -r $target ./;
    cd ..

    if [ -f "$target" ]; then
	rm $target
    fi

    mv "$tmpfolder/$target" .

    echo "<repacker> Signing the APK"
    jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore "$keydir/$keystore" $target $keyalias
    
    echo "<repacker> Cleaning tmp"
    rm -rf $tmpfolder

    echo "<repacker> Done."    

    exit 0
fi
