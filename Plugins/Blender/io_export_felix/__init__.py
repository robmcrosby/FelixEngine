
bl_info = {
    "name":        "Felix Engine Export Suit",
    "author":      "Robert Crosby",
    "version":     (0, 0, 1),
    "blender":     (2, 79, 0),
    "location":    "File > Export",
    "description": "Export Felix Engine Files",
    "category":    "Import-Export"
    }

import bpy
from bpy.props import (
        BoolProperty,
        FloatProperty,
        StringProperty,
        EnumProperty,
        )
from bpy_extras.io_utils import (
        ImportHelper,
        ExportHelper,
        orientation_helper_factory,
        path_reference_mode,
        axis_conversion,
        )

from bpy_extras.io_utils import ExportHelper

class ExportFelixMeshes(bpy.types.Operator, ExportHelper):
    bl_idname       = "export_felix_binary_mesh.mesh"
    bl_label        = "Export Binary Mesh"
    bl_options      = {'PRESET'}

    filename_ext    = ".mesh"

    """
    exportIndices = BoolProperty(
            name="Export Indices",
            description="Export an Index Buffer",
            default=True,
            )

    exportNormals = BoolProperty(
            name="Export Normals",
            description="Export a Normal Buffer",
            default=True,
            )

    exportUVs = BoolProperty(
            name="Export UV Maps",
            description="Export any UV Maps",
            default=True,
            )

    exportColors = BoolProperty(
            name="Export Vertex Colors",
            description="Export any Vertex Colors",
            default=False,
            )

    alignBuffers = BoolProperty(
            name="Align Data",
            description="Add buffer values to Align Vertices",
            default=True,
            )

    meshScale = FloatProperty(
            name="Scale",
            min=0.01, max=1000.0,
            default=1.0,
            )

    stripMode = EnumProperty(
            name="Tri Strips",
            items=(('OFF', "Never", "Do not form Triangle Strips"),
                   ('AUTO', "Optimized", "Optimize with Triangle Strips"),
                   ('ON', "Always", "Always form Triangle Strips"),
                   ),
            )
    """

    def execute(self, context):
        from . import export_felix
        keywords = self.as_keywords(ignore=("global_scale",
                                            "check_existing",
                                            "filter_glob",
                                            ))
        return export_felix.save_binary_mesh(context, **keywords)

class ExportFelixScene(bpy.types.Operator, ExportHelper):
    bl_idname       = "export_felix_xml_mesh.xml"
    bl_label        = "Export Felix Scene"
    bl_options      = {'PRESET'}
    filename_ext    = ".xml"

    meshType = EnumProperty(
        name = 'Export Meshes As:',
        items= (
            ('mesh', 'Binary Files (.mesh)', 'Export as seperate binary mesh files'),
            ('xml', 'XML Files (.xml)', 'Export as seperate xml files'),
            ('json', 'JSON Files (.json)', 'Export as seperate json files'),
            ('embeded', 'Embeded in Scene File', 'Embed meshes in scene file')
        )
    )

    def execute(self, context):
        from . import export_felix
        keywords = self.as_keywords(ignore=("global_scale",
                                            "check_existing",
                                            "filter_glob",
                                            ))
        return export_felix.export_scene(context, **keywords)


def menu_func_meshes(self, context):
    self.layout.operator(ExportFelixMeshes.bl_idname, text="Felix Meshes");

def menu_func_scene(self, context):
    self.layout.operator(ExportFelixScene.bl_idname, text="Felix Scene");

def register():
    bpy.utils.register_module(__name__);
    bpy.types.INFO_MT_file_export.append(menu_func_meshes);
    bpy.types.INFO_MT_file_export.append(menu_func_scene);

def unregister():
    bpy.utils.unregister_module(__name__);
    bpy.types.INFO_MT_file_export.remove(menu_func_meshes);
    bpy.types.INFO_MT_file_export.remove(menu_func_scene);

if __name__ == "__main__":
    register()
