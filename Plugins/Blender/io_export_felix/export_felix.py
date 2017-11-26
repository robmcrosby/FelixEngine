import bpy
import json

import math
import mathutils
from mathutils import *

try:
    import xml.etree.cElementTree as Etree
except ImportError:
    import xml.etree.ElementTree as Etree

import xml.dom.minidom as MiniDom
from struct import pack


################################################################################
##                          JSON File Functions                               ##
################################################################################

def writeJSON(data, filePath):
    with open(filePath, 'w') as outfile:
        json.dump(data, outfile, indent=2)
        outfile.close()


################################################################################
##                          XML File Functions                                ##
################################################################################

def writeXML(node, filePath, pretty = True):
    str = Etree.tostring(node, 'utf-8')
    if pretty:
        reParsed = MiniDom.parseString(str)
        str = reParsed.toprettyxml(indent='  ')
    else:
        str = str.decode("utf-8")
    with open(filePath, 'w') as outfile:
        outfile.write(str)
        outfile.close()

################################################################################
##                      Right to left handed Functions                        ##
################################################################################

def correctVector(v):
    return Vector((v.x, v.z, -v.y))

def correctRotation(q):
    m = Matrix(((1.0, 0.0, 0.0),(0.0, 0.0, 1.0), (0.0, -1.0, 0.0)))
    m = m * q.to_matrix()
    return m.to_quaternion()

################################################################################
##                          Mesh File Functions                               ##
################################################################################

def colorToData(color):
    return [color.r, color.g, color.b]

def vec2ToData(vec):
    return [vec.x, vec.y]

def vec3ToData(vec):
    return [vec.x, vec.y, vec.z]

def quatToData(quat):
    return [quat.x, quat.y, quat.z, quat.w]


################################################################################
##                           Material Functions                               ##
################################################################################

def buildAmbiant(material):
    data = {'type':'Ambiant', 'properties':{}}
    data['properties']['color'] = colorToData(material.diffuse_color)
    data['properties']['factor'] = material.emit
    return data

def buildDiffuse(material):
    data = {'type':'Diffuse', 'properties':{}}
    data['properties']['color'] = colorToData(material.diffuse_color)
    data['properties']['factor'] = material.diffuse_intensity
    return data

def buildSpecular(material):
    data = {'type':'Specular', 'properties':{}}
    data['properties']['color'] = colorToData(material.specular_color)
    data['properties']['factor'] = material.specular_intensity
    data['properties']['hardness'] = material.specular_hardness
    return data

def buildDepthState(material):
    data = {'type':'DepthState', 'properties':{}}
    data['properties']['write'] = 'true'
    data['properties']['function'] = 'less'
    return data

def buildMaterial(material):
    data = {'type':'Material', 'properties':{}, 'data':[]}
    data['properties']['name'] = material.name
    data['properties']['shader'] = 'Shader'
    data['data'].append(buildAmbiant(material))
    data['data'].append(buildDiffuse(material))
    data['data'].append(buildSpecular(material))
    data['data'].append(buildDepthState(material))
    return data


################################################################################
##                             Mesh Functions                                 ##
################################################################################

def getFaceGroups(mesh):
    faceGroups = [[] for i in range(len(mesh.materials))]
    for face in mesh.tessfaces:
        faceGroups[face.material_index].append(face)
    return faceGroups

def buildSubMeshes(mesh):
    data = []
    faceGroups = getFaceGroups(mesh)
    start = 0
    for group in faceGroups:
        end = start
        for face in group:
            end += 3
            if len(face.vertices) > 3:
                end += 3
        data.append({'type':'Range', 'properties':{'start':start, 'end':end}})
        start = end
    return {'type':'SubMeshes', 'data':data}

def buildPositionBuffer(mesh):
    faces = sum(getFaceGroups(mesh), [])
    positions = [vec3ToData(v.co.copy()) for v in mesh.vertices]
    data = []
    for face in faces:
        data.append(positions[face.vertices[0]])
        data.append(positions[face.vertices[1]])
        data.append(positions[face.vertices[2]])
        if len(face.vertices) > 3:
            data.append(positions[face.vertices[0]])
            data.append(positions[face.vertices[2]])
            data.append(positions[face.vertices[3]])
    return {'type':'Buffer', 'properties':{'size':3, 'name':'position'}, 'data':data}

def buildNormalBuffer(mesh):
    faces = sum(getFaceGroups(mesh), [])
    normals = [vec3ToData(v.normal.copy()) for v in mesh.vertices]
    data = []
    for face in faces:
        if face.use_smooth:
            data.append(normals[face.vertices[0]])
            data.append(normals[face.vertices[1]])
            data.append(normals[face.vertices[2]])
            if len(face.vertices) > 3:
                data.append(normals[face.vertices[0]])
                data.append(normals[face.vertices[2]])
                data.append(normals[face.vertices[3]])
        else:
            normal = vec3ToData(correctVector(face.normal))
            data.extend([normal]*3)
            if len(face.vertices) > 3:
                data.extend([normal]*3)
    return {'type':'Buffer', 'properties':{'size':3, 'name':'normal'}, 'data':data}

def buildUVBuffers(mesh):
    buffers = []
    for i, texture in enumerate(mesh.tessface_uv_textures):
        data = []
        for uv in texture.data:
            data.append(vec2ToData(uv.uv1))
            data.append(vec2ToData(uv.uv2))
            data.append(vec2ToData(uv.uv3))
            if len(uv.uv) > 3:
                data.append(vec2ToData(uv.uv1))
                data.append(vec2ToData(uv.uv3))
                data.append(vec2ToData(uv.uv4))
        buffers.append({'type':'Buffer', 'properties':{'size':2, 'name':texture.name}, 'data':data})
    return buffers

def buildColorBuffers(mesh):
    faces = sum(getFaceGroups(mesh), [])
    buffers = []
    for i, colors in enumerate(mesh.tessface_vertex_colors):
        data = []
        for index, color in enumerate(colors.data):
            data.append(colorToData(color.color1))
            data.append(colorToData(color.color2))
            data.append(colorToData(color.color3))
            if len(faces[index].vertices) > 3:
                data.append(colorToData(color.color1))
                data.append(colorToData(color.color3))
                data.append(colorToData(color.color4))
        buffers.append({'type':'Buffer', 'properties':{'size':3, 'name':colors.name}, 'data':data})
    return buffers

def buildMeshData(mesh):
    mesh.calc_tessface()
    mesh.calc_normals()

    data = [buildSubMeshes(mesh)]
    data.append(buildPositionBuffer(mesh))
    data.append(buildNormalBuffer(mesh))
    data.extend(buildUVBuffers(mesh))
    data.extend(buildColorBuffers(mesh))

    return data

def buildMesh(mesh):
    data = {'type':'Mesh', 'properties':{}}
    data['properties']['name'] = mesh.name
    data['data'] = buildMeshData(mesh)
    data['properties']['vertices'] = data['data'][0]['data'][-1]['properties']['end']
    return data


################################################################################
##                            Model Functions                                 ##
################################################################################

def buildTransform(obj):
    rotMode = obj.rotation_mode
    obj.rotation_mode = 'QUATERNION'

    rotation = correctRotation(obj.rotation_quaternion)
    location = correctVector(obj.location)

    data = {'type':'Transform', 'properties':{}, 'data':[]}
    data['data'].append({'type':'Scale', 'data':vec3ToData(obj.scale)})
    data['data'].append({'type':'Rotation', 'data':quatToData(rotation)})
    data['data'].append({'type':'Location', 'data':vec3ToData(location)})

    obj.rotation_mode = rotMode
    return data

def buildModelMaterials(materials):
    data = {'type':'Materials', 'data':[]}
    for index, material in enumerate(materials):
        mat = {'type':'Material', 'properties':{}}
        mat['properties']['name'] = material.name
        mat['properties']['index'] = index
        data['data'].append(mat)
    return data

def buildModel(model):
    data = {'type':'Model', 'properties':{}, 'data':[]}
    data['properties']['name'] = model.name
    data['properties']['pass'] = 'MainPass'
    data['properties']['mesh'] = model.data.name
    data['data'].append(buildTransform(model))
    if len(model.material_slots) == 1:
        data['properties']['material'] = model.material_slots[0].name
    elif len(model.material_slots) > 0:
        data['data'].append(buildModelMaterials(model.material_slots))
    return data


################################################################################
##                            Light Functions                                 ##
################################################################################

def buildSpotLight(light):
    data = {'type':'Spot', 'properties':{}}
    data['properties']['size'] = light.data.spot_size
    data['properties']['blend'] = light.data.spot_blend
    return data

def buildLightFalloff(light):
    data = {'type':'Falloff', 'properties':{}}
    return data

def buildLight(light):
    data = {'type':'Light', 'properties':{}, 'data':[]}
    data['properties']['name'] = light.name
    data['properties']['pass'] = 'MainPass'
    data['properties']['energy'] = light.data.energy
    data['data'].append({'type':'Color', 'data':colorToData(light.data.color)})
    if light.data.type == 'POINT':
        data['properties']['type'] = 'point'
        data['data'].append(buildLightFalloff(light))
    elif light.data.type == 'SUN':
        data['properties']['type'] = 'directional'
    elif light.data.type == 'SPOT':
        data['properties']['type'] = 'spot'
        data['data'].append(buildLightFalloff(light))
        data['data'].append(buildSpotLight(light))
    data['data'].append(buildTransform(light))
    return data


################################################################################
##                            Camera Functions                                ##
################################################################################

def buildClear():
    data = {'type':'Clear', 'properties':{}}
    data['properties']['color'] = '0.2, 0.2, 0.2, 1.0'
    data['properties']['depth'] = '1.0'
    return data

def buildCameraProjection(camera):
    data = {'type':'Projection', 'properties':{}}
    data['properties']['near'] = camera.data.clip_start
    data['properties']['far'] = camera.data.clip_end
    if camera.data.type == 'ORTHO':
        data['properties']['type'] = 'ortho'
        data['properties']['scale'] = camera.data.ortho_scale
    else:
        data['properties']['type'] = 'frustum'
        data['properties']['angle'] = camera.data.angle
    return data

def buildCamera(camera):
    data = {'type':'Camera', 'properties':{}, 'data':[]}
    data['properties']['name'] = camera.name
    data['properties']['pass'] = 'MainPass'
    data['properties']['frame'] = 'MainFrame'
    data['data'].append(buildCameraProjection(camera))
    data['data'].append(buildTransform(camera))
    #TODO figure out how to control clear operations
    data['data'].append(buildClear())
    return data


################################################################################
##                            Scene Functions                                 ##
################################################################################

def buildMainFrame():
    data = {'type':'Frame', 'properties':{}, 'data':[]}
    data['properties']['name'] = 'MainFrame'
    data['properties']['window'] = '0'
    data['data'].append({'type':'Buffer', 'properties':{'type':'depth'}})
    return data

def buildMainShader():
    data = {'type':'Shader', 'properties':{}}
    data['properties']['name'] = 'Shader'
    data['properties']['vertex'] = 'basic_vertex'
    data['properties']['fragment'] = 'basic_fragment'
    return data

def getObjectMaterials(obj):
    materials = []
    for slot in obj.material_slots:
        if slot.material is not None:
            materials.append(slot.material)
    return materials

def buildScene(objects, name):
    meshes = {}
    materials = []
    for obj in objects:
        if obj.type == 'MESH':
            meshes[obj.data.name] = obj.data
            materials.extend(getObjectMaterials(obj))

    data = {'type':'Scene', 'properties':{'name':name}, 'data':[]}

    # TODO: Figure out how to define multiple frames
    data['data'].append(buildMainFrame())

    # TODO: Implement diffrent shaders based on materials
    data['data'].append(buildMainShader())

    for mesh in meshes.values():
        data['data'].append(buildMesh(mesh))

    for material in materials:
        data['data'].append(buildMaterial(material))

    for obj in objects:
        if obj.type == 'MESH':
            data['data'].append(buildModel(obj))
        elif obj.type == 'CAMERA':
            data['data'].append(buildCamera(obj))
        elif obj.type == 'LAMP':
            data['data'].append(buildLight(obj))
    return data


################################################################################
##                        XML Conversion Functions                            ##
################################################################################

def toStr(data, seperator = ', '):
    if type(data) is float:
        return '{:f}'.format(data)
    if type(data) is int:
        return '{:d}'.format(data)
    if type(data) is list:
        return seperator.join(toStr(i) for i in data)
    return data

def isSubData(data):
    if type(data) is list and len(data) > 0:
        return type(data[0]) is dict
    return False

def isIndentedList(data):
    return type(data) is list and len(data) > 0 and type(data[0]) is list

def buildXML(data, level = 0):
    node = Etree.Element(data['type'])
    if 'properties' in data.keys():
        for key in data['properties']:
            node.set(key, toStr(data['properties'][key]))
    if 'data' in data.keys():
        if isSubData(data['data']):
            for item in data['data']:
                node.append(buildXML(item, level+1))
        elif isIndentedList(data['data']):
            indent = '\t'*level
            node.text = '\n'+indent+'\t'
            node.text += toStr(data['data'], ',\n'+indent+'\t')
            node.text += '\n'+indent
        else:
            node.text = toStr(data['data'])
    return node


################################################################################
##                      Binary Mesh Conversion Functions                      ##
################################################################################

# Structure of a Binary Mesh file.
# Note: All values are writen in Big Endien.
#
#{int} primitive type:
#	triangles                 = 0
#	triangle strip            = 1
#	triangle fan              = 2
#	indexed triangles         = 8
#	indexed triangle strip    = 9
#	indexed triangle fan      = 10
#
#{int} number of sub-meshes
#
#{int} sub-mesh start index
#{int} sub-mesh end index (last index + 1)
#...
#
#{int} number of vertices
#{int} number of buffers
#
#{int}      size of buffer components
#{char[32]} buffer name
#{float...} buffer data
#...
#
#{int}    number of indices
#{int...} index data

def packData(data):
    if type(data) is list:
        buffer = b''
        for item in data:
            buffer += packData(item)
        return buffer
    if type(data) is float:
        return pack('!f', data)
    if type(data) is int:
        return pack('!i', data)
    return b''

def writeBinaryMesh(mesh, filePath):
    with open(filePath, 'wb') as outfile:
        subMeshes = mesh['data'][0]['data']
        outfile.write(pack('!i', 0))
        outfile.write(pack('!i', len(subMeshes)))
        for subMesh in subMeshes:
            outfile.write(pack('!i', subMesh['properties']['start']))
            outfile.write(pack('!i', subMesh['properties']['end']))
        outfile.write(pack('!i', mesh['properties']['vertices']))
        outfile.write(pack('!i', len(mesh['data'])-1))
        for buffer in mesh['data'][1:]:
            outfile.write(pack('!i', buffer['properties']['size']))
            outfile.write(pack('32s', buffer['properties']['name'].encode()))
            outfile.write(packData(buffer['data']))
        outfile.close()


################################################################################
##                           Mesh Export Functions                            ##
################################################################################

def exportMesh(mesh, filePath, fileName, fileType):
    fullPath = filePath+'/'+fileName+'.'+fileType

    if fileType == 'xml':
        node = buildXML(mesh)
        writeXML(node, fullPath, True)
    elif fileType == 'json':
        writeJSON(mesh, fullPath)
    elif fileType == 'mesh':
        writeBinaryMesh(mesh, fullPath)

def exportMeshes(meshes, filePath, fileType):
    for mesh in meshes:
        fileName = mesh['properties']['name']
        exportMesh(mesh, filePath, fileName, fileType)
        mesh['properties']['file'] = fileName+'.'+fileType
        mesh['properties'].pop('vertices', None)
        mesh.pop('data', None)


################################################################################
##                           Scene Export Functions                           ##
################################################################################

def getMeshesFromScene(scene):
    meshes = []
    for item in scene['data']:
        if item['type'] == 'Mesh':
            meshes.append(item)
    return meshes

def exportScene(objects, filePath, fileType, sceneName, meshType):
    scene = buildScene(objects, sceneName)

    if meshType == 'xml' or meshType == 'json' or meshType == 'mesh':
        meshes = getMeshesFromScene(scene)
        exportMeshes(meshes, filePath, meshType)

    fullPath = filePath+'/'+sceneName+'.'+fileType
    if fileType == 'xml':
        node = buildXML(scene)
        writeXML(node, fullPath, True)
    elif fileType == 'json':
        writeJSON(scene, fullPath)





def save_binary_mesh(context, filepath="",
                     stripMode     = 'OFF',
                     alignBuffers  = True,
                     exportIndices = True,
                     exportNormals = True,
                     exportUVs     = True,
                     exportColors  = False,
                     meshScale     = 1.0):
    mesh = context.active_object.data
    transform = mathutils.Matrix()
    transform[0][0] = transform[1][1] = meshScale
    transform[2][2] = -meshScale
    writeBinaryMeshFile(mesh, filepath,
                        transform,
                        stripMode,
                        alignBuffers,
                        exportIndices,
                        exportNormals,
                        exportUVs,
                        exportColors)
    return {'FINISHED'}

def save_xml_mesh(context, filepath="",
                  stripMode     = 'OFF',
                  alignBuffers  = True,
                  exportIndices = True,
                  exportNormals = True,
                  exportUVs     = True,
                  exportColors  = False,
                  meshScale     = 1.0):
    mesh = context.active_object.data
    transform = mathutils.Matrix()
    transform[0][0] = transform[1][1] = meshScale
    transform[2][2] = -meshScale
    writeXmlMeshFile(mesh, filepath,
                     transform,
                     stripMode,
                     alignBuffers,
                     exportIndices,
                     exportNormals,
                     exportUVs,
                     exportColors)
    return {'FINISHED'}
