# Point Cloud Viewer

[![Build Status TensorFlow](https://ci.patwie.com/api/badges/cgtuebingen/pointcloud-viewer/status.svg)](http://ci.patwie.com/cgtuebingen/pointcloud-viewer)

This tool allows viewing huge point clouds.

## Build on Ubuntu

Install dependencies

    sudo apt install build-essential cmake
    sudo apt install qt5-default libqt5opengl5-dev libboost-all-dev

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

## Usage: Basics

Load a Point Cloud with the menu bar:

> **Project > Import Pointcloud**

There are two available navigation schemes for navigating the 3d view:

- Blender  
    **View > Navigation > Blender**
- MeshLab  
    **View > Navigation > MeshLab**

### Blender Inspired Navigation Scheme

In the Blender Navigation scheme, you will see a grid:

![screenshot](doc/images/navigation_blender_grid.png)

You have three different ways to navigate:

- **First Person Navigation**  
    This Navigation resembles the movement of a first person shooter game.

    1. Enter the first person navigation by pressing ![the shift key](doc/images/key_shift.svg)+![the f key](doc/images/key_f.svg)
    2. Move around
        - Look around by moving your mouse
        - Move forward/backward by holding ![the w key](doc/images/key_w.svg)/![s key](doc/images/key_s.svg)
        - Move left/right by holding ![the a key](doc/images/key_a.svg)/![d key](doc/images/key_d.svg)
        - Move up/down by holding ![the e key](doc/images/key_e.svg)/![q key](doc/images/key_q.svg)
        - You can roll the camera by scrolling with ![your mouse wheel](doc/images/button_middle.svg)
            - To reset the rolling, just press ![the middle mouse button](doc/images/button_middle.svg) while holding ![the ctrl key](doc/images/key_ctrl.svg).
    3. Leave the first person navigation
        - Keeping the new position by pressing ![the enter key](doc/images/key_enter.svg) or ![the left mouse button](doc/images/button_left.svg)
        - Undo the position and reset to the old one by pressing ![the esc key](doc/images/key_esc.svg) or ![the right mouse button](doc/images/button_right.svg)
- **Turntable Navigation**  
    This Navigation allows a quickly switching the perspective on the point cloud.

    - For turning the view around the object, hold ![the middle mouse button](doc/images/button_middle.svg) and move your mouse.
    - For shifting the view orthogonal to the view direction, hold ![the shift key](doc/images/key_shift.svg) and ![the middle mouse button](doc/images/button_middle.svg) and move your mouse.
    - For changing the distance to the object, scroll with ![your mouse wheel](doc/images/button_middle.svg) or hold ![the ctrl key](doc/images/key_ctrl.svg) and ![the middle mouse button](doc/images/button_middle.svg) and move your mouse.
- **Selecting Points**  
    You can quickly select a point and move towards it.

    - To select a single point, right click it ![with the right mouse button](doc/images/button_right.svg).
    - To center the view around the selected point, press ![](doc/images/key_comma.svg) (the comma key).

### MeshLab Inspired Navigation Scheme

In the Blender Navigation scheme, you will see a trackball:

![screenshot](doc/images/navigation_meshlab_trackball.png)

You have three different ways to navigate:

- **First Person Navigation**  
    - Usage is the same as in the Blender Navigation Scheme
- **Trackball Navigation**  
    This Navigation allows a quickly switching the perspective on the point cloud.

    - For turning the view around the object, rotate the trackball by moving it with the mouse while holding ![the left mouse button](doc/images/button_left.svg).
    - For shifting the view orthogonal to the view direction, move the trackball with the mouse while holding ![the ctrl key](doc/images/key_ctrl.svg) and ![the left mouse button](doc/images/button_left.svg).
    - For changing the distance to the object, scroll with ![your mouse wheel](doc/images/button_middle.svg) or move the trackball with the mouse while holding ![the shift key](doc/images/key_shift.svg) and ![the left mouse button](doc/images/button_left.svg).
- **Selecting Points**  
    You can quickly select a point and move towards it.

    - To select a single point, right click it ![with the right mouse button](doc/images/button_right.svg).
    - To center the view around a point, double click it with ![the left mouse button](doc/images/button_left.svg) or ![the middle mouse button](doc/images/button_middle.svg)

Additionally, you can change the render properties:

- To change the point rendering size, hold ![the alt key](doc/images/key_alt.svg) and scroll with ![your mouse wheel](doc/images/button_middle.svg)


## Usage: Animation

You can animate the camera by defining a camera path.
The path consists out of multiple key-points.

If you have already a ready to use camera path, you can import it with the menu bar:  

> **Flythrough > Import Path**

Alternatively, you can create your own path by navigating the camera and add a new key-point by pressing ![the i key](doc/images/key_i.svg).

More options for changing the flythrough camera animation can be found in the **Flythrough-Tab** (red):

![screenshot](doc/images/tab_flythrough_overview.png)

- With the **Flythrough Menu** (yellow), you can
    - export the current flythrough path
    - import an already created flythrough path
- In the **Keypoints List** (blue), you can reorder keypoints and remove unwanted keypoints.
- In the **Playback Area** (green), you can modify the
    - duration of the whole flythrough
    - velocity of the camera
    - interpolation between points

## Usage: Render

You can render the frames of the animation to single png files. To do this, go the the **Render-Tab** (red):

![screenshot](doc/images/tab_render.png)

- With the **Render Button** (blue), you start the Rendering Process and start the rendering process. If this button is grayed out, you need to create a key-point for the animation (you can press ![the i key](doc/images/key_i.svg) for creating a single key-point at the current camera location).
- You can customize the style
  - Change the brightness of the background (0 is black, 255 is white and 54 the default brightness)
  - Change the point size



## Typical usage: Data Inspection

You can render the frames of the animation to single png files. To do this, go the the **Data-Inspection-Tab** (red):

![screenshot](doc/images/tab_data_inspection.png)

Here, you can

- Build the KD-Tree (blue)
- See information of the selected point (green)

In order to be able to pick single points, you need to build the KD-Tree first. You can to this with the bug **Unlock Point Picker** button. In order to automatically build the KD-Tree every time you load a point-cloud, check the **Automatically Unlock after loading** checkbox.

If you try to pick a point (by clicking ![the right mouse button](doc/images/button_right.svg)) without building the KD-Tree first, a remonder dialog will open and allow you to immediately build the KD-Tree:  
![screenshot](doc/images/dialog_build_kdtree_now.png)

## Known Issues

Currently, no optimization technique is applied. When the pointcloud size exceeds the available video memory, the application will probably crash.
