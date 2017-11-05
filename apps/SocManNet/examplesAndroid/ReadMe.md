# Using Source Code with Android Studio
## 1. Create a new project
After starting Android Studio, create a new project. The default application name is *MyApplication*. For the examples here the application names are that of the folders (e.g. *smnTwitter*). Because that happens automaticly with Android Studio: A subfolder in your project environment will be created with the name of the application.

We used *mplab.hsh* as company domain. *mplab* stands for Mikrocontrollerlabor (microcontroller laboratory) and *hsh* for Hochschule Hannover (University of Applied Sciences and Arts Hannover). That makes the package name hsh.mplab.smntwitter for the example smnTwitter.

Next select form factor *Phone and Tablet* and the minimum SDK (which is the oldest version of Android your application should be usable). With Android Studio 3.0 it is not possible to set API older 14 here, but You can do that later in the settings of the project.

Next select *Empty Activity*, which is the default, and let also the defaults be *MainActivity* for activity name and *activity_main* for layout name. 

Now a project is created with a folder structure like this:
* .../smnTwitter
  * app
    * src
      * main
        * java
          * hsh
            * mplab
              * smntwitter
                * ***MainActivity.java***
              * *... here we later add the folders for the libraries*
        * res
          * layout
            * ***activity_main.xml***
          
## 2. Add the library files
Create a folder with the name of your library, but only small letters (e.g. *socmannet*) on the level of your projects java source folder (e.g. beside folder *smntwitter*) and copy the .java files from this repository (e.g. *Follower.java*, *SocManNet.java* and *Twitter.java*) into the new folder.

## 3. Replace or edit application files
If you run a similar version as I did (Android Studio 3.0 with compile level 26), you may simply replace the files *MainActivity.java* and *activity_main.xml* with the files from this repository (e.g. from folder *smnTwitter*).

But if You see differences in object and function names of the files in Your new project compared with the files from this repository, then copy the extra code from the repository files into the files of Your new project.

## 4. Adapt to your domain
If you use your own company domain (e.g. *mycompany.mycountry* or *mydevision.mycompany*) when creating the project, you'll find these names in the folder tree above. But You must change the packet names and imports in the sources (library files and application files) accordingly.
