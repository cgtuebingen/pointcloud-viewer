# Point Cloud Viewer

[![Build Status TensorFlow](https://ci.patwie.com/api/badges/cgtuebingen/pointcloud-viewer/status.svg)](http://ci.patwie.com/cgtuebingen/pointcloud-viewer)

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

## Typical usage: Animation

1. Load a Point Cloud with the menu bar:
    > **Project > Import Pointcloud**
2. You can navigate with
    - Blender style navigation:
        - turntable navigation
            - mouse wheel for zooming            
            - hold `middle mouse button` and move mouse for rotating around the origin
            - hold `middle mouse button` and hold `shift` and move mouse for shifting the camera
            - hold `middle mouse button` and hold `ctrl` and move mouse for zooming the camera
        - Enter FPS mode with `Shift`+`F`
            - Leave FPS mode with `Esc` or `Right Mouse Button` resetting the camera
            - Leave FPS mode with `Return` or `Left Mouse Button` to keep the new camera location
            - Move the camera up and down with `E` and `Q`
            - Change movement speed with the mouse wheel
            - Tilt the camera by scrolling the mouse wheel while holding `Ctrl` (hold `Ctrl`+`Shift` for faster tilting).
                - Reset the tilt of the camera by pressing `Ctrl`+`Middle Mouse Button`
3. Either import an already created animation with the menu bar:
    > **Flythrough > Import Path**

    or create a new camera path by navigating manually and pressing the key `I`
4. In the left bar, you can change the camera velocity
5. In the left bar, you can also change the render settings (background color and point size)
6. In the left bar, you can start the Render process by pressing the big Render button
    - It is insensitive as long as there is no camera path
    - Currently, only one image per frame is saved

## Typical usage: Analyze PointData

1. Go to the Data Inspection Tab
2. If the Kd Tree is not already built, you can build not the kd tree by using the big "Unlock Point Picker button". If the button is greyed out, the kd tree is alreada built.
3. You can pick single points by clicking on them with the left mouse button

## Known Issues

Currently, no optimization technique is applied. When the pointcloud size exceds the available video memory, the application will probably crash.
