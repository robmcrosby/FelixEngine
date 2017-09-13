//
//  FelixEngine.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/11/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import <UIKit/UIKit.h>

//! Project version number for FelixEngine.
FOUNDATION_EXPORT double FelixEngineVersionNumber;

//! Project version string for FelixEngine.
FOUNDATION_EXPORT const unsigned char FelixEngineVersionString[];


// Obj-C Headers
#import <FelixEngine/iOSViewController.h>

// C++ Headers
#import <FelixEngine/Application.h>
#import <FelixEngine/FileSystem.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicResources.h>
#import <FelixEngine/GraphicStates.h>
#import <FelixEngine/GraphicTask.h>
#import <FelixEngine/TrackerSystem.h>
#import <FelixEngine/Vector.h>
#import <FelixEngine/Matrix.h>
#import <FelixEngine/Quaternion.h>
#import <FelixEngine/Variant.h>
#import <FelixEngine/IndexedMap.h>
#import <FelixEngine/XMLTree.h>
#import <FelixEngine/StringUtils.h>
#import <FelixEngine/VertexMeshData.h>
#import <FelixEngine/ImageBufferData.h>
#import <FelixEngine/UniformMap.h>
#import <FelixEngine/TextureMap.h>
#import <FelixEngine/Event.h>
#import <FelixEngine/EventListener.h>
#import <FelixEngine/EventListenerSet.h>
#import <FelixEngine/EventHandler.h>
#import <FelixEngine/EventSubject.h>

#import <FelixEngine/Scene.h>
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/Model.h>
#import <FelixEngine/Camera.h>
#import <FelixEngine/Material.h>
#import <FelixEngine/Light.h>

// iOS Platform
#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>
#import <FelixEngine/ARKitTracker.h>

