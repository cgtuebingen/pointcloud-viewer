# Point Cloud Viewer

This tool allows viewing huge point clouds.

## Build on Ubuntu

Install dependencies

    sudo apt install build-essential cmake
    sudo apt install qt5-default libboost-all-dev

Clone with

    git clone git@github.com:cgtuebingen/pointcloud-viewer.git
    cd pointcloud-viewer/
    git submodule update --init
    
Compile with
    
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ../src
    make -j8
    
Run with

    pointcloud_viewer/pointcloud_viewer

## Typical usage

1. Load a Point Cloud with the menu bar:
    > **Project > Import Pointcloud**
2. You can navigate with
    - Blender style turntable navigation
        - hold `middle mouse button` and move mouse for rotating around the origin
        - hold `middle mouse button` and hold `shift` and move mouse for shifting the camera
        - hold `middle mouse button` and hold `ctrl` and move mouse for zooming the camera
        - **Known issue: origin of the turntable is currently placed unintituively**
    - Enter FPS mode with `Shift`+`F`, leave with `Esc`
3. Either import an already created animation with the menu bar:
    > **Flythrough > Import Path**
    
    or create a new camera path by navigating manually and pressing the key `I`
4. In the left bar, you can change the camera velocity
5. In the left bar, you can also change the render settings (background color and point size)
6. In the left bar, you can start the Render process by pressing the big Render button
    - It is insensitive as long as there is no camera path
    - Currently, only one image per frame is saved
    
## Known Issues

Currently, no optimization technique is applied. When the pointcloud size exceds the available video memory, the application will probably crash. 
