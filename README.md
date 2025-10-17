# asset-cacher
A caching tool for generating .dat asset files used in the Electronic Arts' Lord of the Rings: Battle for Middle-earth game series.

## Key features

* A fast console-based Windows application;
* Fully portable on Windows, does not rely on any additional libraries;
* Can generate both standalone asset caches and add files to an existing cache;

## Brief overview of the .dat format

The application generates .dat files, which contain information on assets used by the game.  Specifically, the cache has two parts: the first lists names, creation times, sizes and structures of various assets (textures and models).  The second stores data on each asset's dependencies/inputs - that is, other assets used by the current asset (for example, a the surface of a mesh within a model is covered a texture from a different asset).

## Settings overview

* **Incremental** = **true/false**: should a cache be merged with an existing cache (true) or made standalone (false)?  The default setting is **false**;
* **Input policy** = **Relaxed/Informative/Pedantic**: what should be done if an input is encountered which points to an asset not in the cache? If the policy is **Relaxed**, this fact is ignored; if **Informative**, a warning is printed; if **Pedantic**, the input is omitted from the cache.  The default setting is **Informative**;
* **Show settings** = **true/false**: should the settings menu be shown upon the application's start from the next launch on?  The default setting is **true**.  N.B. If settings are hidden and need to be changed, the settings file **settings.json** needs to be amended directly: the line _"Show options": false_ has to be changed to _"Show options": true_ (or removed altogether). The settings file is in the working directory (where the application file is located);

## Working with the application

* The application processes files stored in the working directory and all its subfolders.  It ignores all files that are either not textures (.dds, .jpg/.jpeg, .png, .tga) or not game models (.w3d);
* If the cache is formed incrementally using information from an existing file, it is expected to be in the working directory named as asset.dat.  In the absence of such a file, the application behaves in the same way as if a standalone cache is generated;
* The newly formed cache is saved as asset.dat in the working directory.  An already exisitng asset.dat file (if there is one) is renamed to asset.dat.bak;

## Future development plans

* Support for extracting files directly from EA's archives (the .big format) is planned in future releases;
