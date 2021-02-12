![RTS Camera logo](/Resources/Icon128.png)

# MWSmoothCamera
An RTS-like smooth camera plugin for Unreal Engine

## Required Setup

1. Download plugin and place in the Plugins directory of your project
2. Restart your editor (you will likely be prompted)
3. Create your own custom GameMode blueprint so you can override the default Pawn and PlayerController Blueprints
4. Create a new blueprint that extends PlayerController
    * Set `Show Mouse Cursor` to true so the cursor remains visible in game
5. Create a new blueprint that extends Pawn
    * In the components section (top of left sidebar), add a new MWSmoothCameraComponent to your default scene root
6. In the World Settings for your level, override the GameMode with yours
    * Set the PlayerController to your new PlayerController blueprint
    * Set the Pawn to your new Pawn blueprint
7. In your Editor Settings -> Input add the following input mappings:

|Name  | Input |
| ------------- | ------------- |
| RotateCamera  | Middle Mouse Button  |

8. Add the following axis mappings as well:

#### MoveCameraLeftRight
|Input  | Axis |
| ------------- | ------------- |
| Right  | 1.0  |
| Left  | -1.0  |
| D  | 1.0  |
| A  | -1.0  |

#### MoveCameraUpDown
|Input  | Axis |
| ------------- | ------------- |
| Up  | 1.0  |
| Down  | -1.0  |
| W  | 1.0  |
| S  | -1.0  |

#### ZoomCamera
|Input  | Axis |
| ------------- | ------------- |
| Mouse Wheel Axis  | -1.0  |

8. In your PlayerController _or_ your Pawn, connect your newly defined input events with the corresponding MWSmoothCameraComponent methods
    * Note: if you handle input in your player controller, you will need to get your pawn and then get it's MWSmoothCameraComponent
    
| Input Binding  | Method |
| ------------- | ------------- |
| RotateCamera (pressed)  | RotateCameraPressed  |
| RotateCamera (released)  | RotateCameraReleased  |
| MoveCameraLeftRight | MoveCameraLeftRight  |
| MoveCameraUpDown | MoveCameraUpDown  |
| ZoomCamera | ZoomCamera  |

Note: make sure to connect the Axis value in the blueprint to the MWSmoothCameraComponent method!

9. Press Play to use your new RTS camera!

## C++ Support

The plugin is coded in C++ and exposed to Blueprints, so all the same methods are available in C++.

## FAQ

1. I start playing the game and my camera immediately starts moving in a direction, why?
    * This is likely because you have not mapped all the input bindings to MWSmoothCamera methods. Right now a known limitation requires you to map all the above mentioned methods in order to work properly.
    
## Known Limitations
* Assumed the plugin is used on a desktop Windows game
* Assumes the player is using a keyboard and mouse
* If MoveCameraLeftRight and MoveCameraUpDown methods are not bound to the correct inputs, the camera will start moving on play
