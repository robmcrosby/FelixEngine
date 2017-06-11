

import os
import time

import bpy
import mathutils
import bpy_extras.io_utils

from struct import *

try:
    import xml.etree.cElementTree as Etree
except ImportError:
    import xml.etree.ElementTree as Etree




################################################################################
##                            File Functions                                  ##
################################################################################

def getDirOfPath(path):
    """ Strips the file from the given Path """
    dirs = path.split('/')
    dirs.pop()
    ret = ''
    for dir in dirs:
        ret += dir + '/'
    return ret

def readXml(filePath, rootName):
    """ Reads an xml file and returns the root node. """
    try:
        tree = Etree.parse(filePath)
        node = tree.getroot()
        if node.tag == root:
            return node;
        return Etree.Element(rootName)
    except FileNotFoundError:
        return Etree.Element(rootName)


def writeXml(rootNode, filePath):
    """ Writes the rootNode to the filePath. """
    tree = Etree.ElementTree(rootNode)
    tmpFile = getDirOfPath(filePath) + 'tmp.xml'
    tree.write(tmpFile)
    # TODO: Dangerous Operations! Need a better solution.
    os.system('xmllint --format ' + tmpFile + ' > ' + filePath)
    os.system('rm -f ' + tmpFile)


def createDir(rootPath, dirName):
    dirPath = rootPath + dirName + '/'
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)

def clearDir(dirPath):
    """ Clears the directory. """
    # TODO: Dangerous Operation! Need a better solution.
    os.system('rm -f -r ' + dirPath + '*')




################################################################################
##                         XML Node Functions                                 ##
################################################################################

def getSubNode(node, name):
    """ Gets or Creates a subNode in a node by the given name. """ 
    subNode = node.find(name)
    if subNode == None:
        subNode = Etree.SubElement(node, name)
    return subNode





################################################################################
##                        Mesh Buffer Functions                               ##
################################################################################

def roundVector(vector, places):
    ret = vector.copy()
    for i in range(len(ret)):
        ret[i] = round(ret[i], places)
    return ret

def addVector(vectors, vectorMap, vector):
    key = roundVector(vector, 2).freeze()
    if key not in vectorMap:
        vectorMap[key] = len(vectors)
        vectors.append(vector)
    return vectorMap[key]

def extractMeshBuffers(mesh, bufferMap, indexMap,
                       extractNormals = True,
                       extractUVs = True, 
                       extractColors = False):
    mesh.calc_tessface()
    mesh.calc_normals()
    
    # Get the vertex positions and normals
    positions = list()
    normals   = list()
    normalMap = dict()
    for vertex in mesh.vertices:
        positions.append(vertex.co)
        normals.append(vertex.normal)
    
    # Setup the UV lists
    uvNames  = list()
    uvCoords = list()
    uvMaps   = list()
    if extractUVs:
        for uv in mesh.tessface_uv_textures:
            uvNames.append(uv.name)
            uvCoords.append(list())
            uvMaps.append(dict())
    
    # Setup the Color lists
    colorNames = list()
    colors     = list()
    colorMaps  = list()
    if extractColors:
        for color in mesh.tessface_vertex_colors:
            colorNames.append(color.name)
            colors.append(list())
            colorMaps.append(dict())
    
    # Get the mesh faces in seperate materals
    facelists = list()
    for i in range(max(1, len(mesh.materials))):
        facelists.append(list())
    for face in mesh.tessfaces:
        facelists[face.material_index].append(face)
    
    # Get Vertex Tuples, UVs, Colors, and Face Normals from the Faces
    vertices = list()
    for i, facelist in enumerate(facelists):
        vertices.append(list())
        for faceIndex, face in enumerate(facelist):
            if len(face.vertices) >= 3:
                v1 = (face.vertices[0],)
                v2 = (face.vertices[1],)
                v3 = (face.vertices[2],)
                if extractNormals:
                    if face.use_smooth:
                        v1 += v1
                        v2 += v2
                        v3 += v3
                    else:
                        nIndex = addVector(normals, normalMap, face.normal)
                        v1 += (nIndex,)
                        v2 += (nIndex,)
                        v3 += (nIndex,)
                for uvIndex, uvCoordsList in enumerate(uvCoords):
                    v1 += (addVector(uvCoordsList, uvMaps[uvIndex], mesh.tessface_uv_textures[uvIndex].data[faceIndex].uv1),)
                    v2 += (addVector(uvCoordsList, uvMaps[uvIndex], mesh.tessface_uv_textures[uvIndex].data[faceIndex].uv2),)
                    v3 += (addVector(uvCoordsList, uvMaps[uvIndex], mesh.tessface_uv_textures[uvIndex].data[faceIndex].uv3),)
                for colorIndex, colorsList in enumerate(colors):
                    v1 += (addVector(colorsList, colorMaps[colorIndex], mesh.tessface_vertex_colors[colorIndex].data[faceIndex].color1),)
                    v2 += (addVector(colorsList, colorMaps[colorIndex], mesh.tessface_vertex_colors[colorIndex].data[faceIndex].color2),)
                    v3 += (addVector(colorsList, colorMaps[colorIndex], mesh.tessface_vertex_colors[colorIndex].data[faceIndex].color3),)
                vertices[i].append(v1)
                vertices[i].append(v2)
                vertices[i].append(v3)
            if len(face.vertices) >= 4:
                v1 = (face.vertices[0],)
                v2 = (face.vertices[2],)
                v3 = (face.vertices[3],)
                if extractNormals:
                    if face.use_smooth:
                        v1 += v1
                        v2 += v2
                        v3 += v3
                    else:
                        nIndex = addVector(normals, normalMap, face.normal)
                        v1 += (nIndex,)
                        v2 += (nIndex,)
                        v3 += (nIndex,)
                for uvIndex, uvCoordsList in enumerate(uvCoords):
                    v1 += (addVector(uvCoordsList, uvMaps[uvIndex], mesh.tessface_uv_textures[uvIndex].data[faceIndex].uv1),)
                    v2 += (addVector(uvCoordsList, uvMaps[uvIndex], mesh.tessface_uv_textures[uvIndex].data[faceIndex].uv3),)
                    v3 += (addVector(uvCoordsList, uvMaps[uvIndex], mesh.tessface_uv_textures[uvIndex].data[faceIndex].uv4),)
                for colorIndex, colorsList in enumerate(colors):
                    v1 += (addVector(colorsList, colorMaps[colorIndex], mesh.tessface_vertex_colors[colorIndex].data[faceIndex].color1),)
                    v2 += (addVector(colorsList, colorMaps[colorIndex], mesh.tessface_vertex_colors[colorIndex].data[faceIndex].color3),)
                    v3 += (addVector(colorsList, colorMaps[colorIndex], mesh.tessface_vertex_colors[colorIndex].data[faceIndex].color4),)
                vertices[i].append(v1)
                vertices[i].append(v2)
                vertices[i].append(v3)
    
    # Initalize the buffers
    bufferMap["Position"] = list()
    if extractNormals:
        bufferMap["Normal"] = list()
    for name in uvNames:
        bufferMap[name] = list()
    for name in colorNames:
        bufferMap[name] = list()
    
    # Compile everything into the buffers
    uvOffset = 2 if extractNormals else 1
    colOffset = uvOffset + len(uvNames)
    vertexMap = dict()
    for verticesList in vertices:
        indices = list()
        for v in verticesList:
            if v not in vertexMap:
                vertexMap[v] = len(vertexMap)
                bufferMap["Position"].append(positions[v[0]])
                if extractNormals:
                    bufferMap["Normal"].append(normals[v[1]])
                for i, uvs in enumerate(uvCoords):
                    bufferMap[uvNames[i]].append(uvs[v[uvOffset+i]])
                for i, colorsList in enumerate(colors):
                    bufferMap[colorNames[i]].append(colorsList[v[colOffset+i]])
            indices.append(vertexMap[v])
        indexMap.append(indices)
    

def deIndexBufferMap(bufferMap, indexMap):
    map = dict()
    for name in bufferMap.keys():
        map[name] = list()
    for indices in indexMap:
        for i in indices:
            for name in bufferMap.keys():
                map[name].append(bufferMap[name][i])
    return map

def transformBufferMap(bufferMap, transform):
    if 'Position' in bufferMap:
        for i, pos in enumerate(bufferMap['Position']):
            bufferMap['Position'][i] = transform * pos
    if 'Normal' in bufferMap:
        for i, norm in enumerate(bufferMap['Normal']):
            norm = transform * norm
            bufferMap['Normal'][i]= norm.normalized()

def printVector(vector, align):
    ret = ''
    for v in vector:
        ret += '{:9.4f}, '.format(v)
    if align and len(vector) == 3:
        ret += '   1.0000,'
    return ret



################################################################################
##                       Binary Mesh File Functions                           ##
################################################################################

# Structure of a Binary Mesh file.
# Note: All values are writen in Big Endien.
#
#{int} primitive type:
#	triangles                 = 0
#	triangle strip            = 1
#	indexed triangles         = 8
#	indexed triangle strip    = 9
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


def writeBinaryMeshFile(mesh, filePath,
                   transform     = mathutils.Matrix(),
                   stripMode     = 'OFF',
                   alignBuffers  = True,
                   exportIndices = True,
                   exportNormals = True,
                   exportUVs     = True,
                   exportColors  = False):
    bufferMap = dict()
    indexMap = list()
    extractMeshBuffers(mesh, bufferMap, indexMap,
                       exportNormals,
                       exportUVs,
                       exportColors)
    transformBufferMap(bufferMap, transform)
    # TODO: Add Triangle Stripping Function here
    if not exportIndices:
        bufferMap = deIndexBufferMap(bufferMap, indexMap)
    
    meshFile = open(filePath, 'wb')
    
    # Write the primative type
    # TODO: Implement more options with Triangle Strips
    triType = 8 if exportIndices else 0
    meshFile.write(pack('!i', triType))
    
    # Write the Sub-Meshes and build the Index Buffer
    indexCount = 0
    indexBuffer = b''
    meshFile.write(pack('!i', len(indexMap)))
    for indices in indexMap:
        meshFile.write(pack('!i', indexCount))
        for index in indices:
            indexBuffer += pack('!i', index)
        indexCount += len(indices)
        meshFile.write(pack('!i', indexCount))
    
    meshFile.write(pack('!i', len(bufferMap['Position'])))
    meshFile.write(pack('!i', len(bufferMap)))
    
    for attribute, buffer in bufferMap.items():
        compSize = len(buffer[0])
        if alignBuffers and compSize == 3:
            compSize = 4
        meshFile.write(pack('!i', compSize))
        meshFile.write(pack('32s', attribute.encode('utf-8')))
        for vector in buffer:
            for v in vector:
                meshFile.write(pack('!f', v))
            for i in range(compSize-len(vector)):
                meshFile.write(pack('!f', 1.0))
    
    # Write the Index Buffer if exporting indices 
    if exportIndices:
        meshFile.write(pack('!i', indexCount))
        meshFile.write(indexBuffer)
    
    meshFile.close()



################################################################################
##                          Mesh Node Functions                               ##
################################################################################

def getMeshes(objects):
    meshes = set()
    for object in objects:
        if object.type == 'MESH':
            meshes.add(object.data)
    return meshes


def addSubMeshes(meshNode, mesh):
    if len(mesh.materials) > 0:
        subMeshesNode = Etree.SubElement(meshNode, 'SubMeshes')
        for index, material in enumerate(mesh.materials):
            subMeshNode = Etree.SubElement(subMeshesNode, 'SubMesh')
            subMeshNode.set('index', '{}'.format(index))
            subMeshNode.set('material', material.name)


def addMesh(resourcesNode, mesh):
    meshNode = Etree.SubElement(resourcesNode, 'Mesh')
    meshNode.set('name', mesh.name)
    meshNode.set('file', mesh.name + '.xml')
    addSubMeshes(meshNode, mesh)


def addMeshes(resourcesNode, objects, path):
    createDir(path, 'Meshes')
    meshPath = path + 'Meshes/'
    
    for mesh in getMeshes(objects):
        addMesh(resourcesNode, mesh)
        writeMeshFile(mesh, meshPath)



################################################################################
##                       Xml Mesh File Functions                           ##
################################################################################

def writeMeshToXml(mesh, meshNode,
                   transform     = mathutils.Matrix(),
                   stripMode     = 'OFF',
                   alignBuffers  = True,
                   exportIndices = True,
                   exportNormals = True,
                   exportUVs     = True,
                   exportColors  = False):
    bufferMap = dict()
    indexMap = list()
    extractMeshBuffers(mesh, bufferMap, indexMap,
                       exportNormals,
                       exportUVs,
                       exportColors)
    transformBufferMap(bufferMap, transform)
    # TODO: Add Triangle Stripping Function here
    if not exportIndices:
        bufferMap = deIndexBufferMap(bufferMap, indexMap)
    
    # TODO: Implement Triangle Stripping
    meshNode.set('primative', 'triangles')
    
    buffersNode = Etree.SubElement(meshNode, 'Buffers')
    buffersNode.set('vertices', '{}'.format(len(bufferMap['Position'])))
    for attribute, buffer in bufferMap.items():
        compSize = len(buffer[0])
        if alignBuffers and compSize == 3:
            compSize = 4
        buffNode = Etree.SubElement(buffersNode, 'Buffer')
        buffNode.set('attribute', attribute)
        buffNode.set('components', '{}'.format(compSize))
        buffNode.text = ''
        for vector in buffer:
            buffNode.text += '\n      ' + printVector(vector, alignBuffers)
        buffNode.text +=  '\n    '
    
    if exportIndices:
        indicesNode = Etree.SubElement(meshNode, 'Indices')
        indicesNode.text = ''
        i = 0
        for indices in indexMap:
            for index in indices:
                if i%9 == 0:
                    indicesNode.text += '\n    '
                elif i%3 == 0:
                    indicesNode.text += '  '
                i += 1
                indicesNode.text += '{:4}, '.format(index)
        indicesNode.text += '\n  '
    
    subMeshesNode = Etree.SubElement(meshNode, 'SubMeshes')
    pos = 0
    for indices in indexMap:
        subMeshNode = Etree.SubElement(subMeshesNode, 'SubMesh')
        subMeshNode.set('start', '{}'.format(pos))
        pos += len(indices)
        subMeshNode.set('end', '{}'.format(pos))


def writeXmlMeshFile(mesh, filePath,
                     transform     = mathutils.Matrix(),
                     stripMode     = 'OFF',
                     alignBuffers  = True,
                     exportIndices = True,
                     exportNormals = True,
                     exportUVs     = True,
                     exportColors  = False):
    meshNode = Etree.Element('Mesh')
    writeMeshToXml(mesh, meshNode,
                   transform,
                   stripMode,
                   alignBuffers,
                   exportIndices,
                   exportNormals,
                   exportUVs,
                   exportColors)
    writeXml(meshNode, filePath)



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
