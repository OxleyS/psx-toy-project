#  PlayStation RSD export plug-in v1.00
#  Written by Lameguy64/TheCodingBrony of Meido-Tek Productions.
#  Any modifications from the original written by Sean Oxley
#
#  Notes:
#   - All polygons of the model must only be triangles (3 point polygons) or
#     quads (4 point polygons).
#   - Textures must be applied through the UV/Image Editor panel as materials are not
#     used since it is less convenient for editing/real-time preview.
#   - UV coords must not exceed the bounds of the texture. Wrapped/tiled textures are
#     not supported.
#   - All associated textures files while modelling must have a filename with no
#     spaces and less than 8 characters long (not including extension) due to RSDLINK
#     being a DOS program. Also, all associated texture files must be converted to
#     PlayStation TIM format with a separate tool and configured with TIMTOOL for the
#     textures to render correctly when the RSD file is converted into a TMD file.
#
#  Known issues:
#   - Sometimes, exported textured quads might have their UV coords rotated by 180
#     degress. This happens very rarely however and can be fixed easily by swapping
#     the UV coords of the glitched quad around.
#
# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####


"""
This script exports PlayStation SDK compatible RSD files from Blender.
It supports normals, colors and textured polygons.
Only one mesh can be exported at a time.
"""

import os
import bpy

from bpy.props import (CollectionProperty,
                       StringProperty,
                       BoolProperty,
                       EnumProperty,
                       FloatProperty,
                       )

from bpy_extras.io_utils import (ImportHelper,
                                 ExportHelper,
                                 axis_conversion,
                                 )


bl_info = {
    "name":         "PlayStation RSD",
    "author":       "Jobert 'Lameguy' Villamor (Lameguy64/TheCodingBrony)",
    "blender":      (2,6,9),
    "version":      (1,0,0),
    "location":     "File > Export",
    "description":  "Export to PlayStation SDK compatible RSD model format",
    "category":     "Import-Export"
}


class ExportRSD(bpy.types.Operator, ExportHelper):
    
    """Export a single object as a PlayStation SDK compatible RSD model file with """ \
    """normals, colors and textured polygons mostly intact"""
    
    bl_idname       = "export_mesh.rsd";
    bl_label        = "Export RSD";
    
    filename_ext    = ".rsd";
    filter_glob		= StringProperty(default="*.rsd;*.ply;*.mat", options={'HIDDEN'})
    
    
    # Export options
    apply_modifiers = BoolProperty(
        name="Apply Modifiers",
        description="Apply Modifiers to the exported mesh",
        default=True,
        )
    
    write_normals = BoolProperty(
        name="Normals",
        description="Export Normals for smooth and "
                    "hard shaded faces, disabling this option will "
                    "disable light source calculation for all polygons",
        default=True,
        )
    
    optimize_normals = BoolProperty(
        name="Optimize Normals",
        description="Optimize vertex normals by discarding "
                    "duplicate values, don't use this option "
                    "if you plan to animate your model using MIME "
                    "vertex animation with normals",
        default=True,
        )
    
    colored_tpolys = BoolProperty(
        name="Colored Textured Faces",
        description="Export all textured faces as vertex colored, "
                    "light source calculation on such faces will be "
                    "disabled however due to library limitations",
        default=False,
        )
    
    translucent_polys = BoolProperty(
        name="Translucent",
        description="Set all exported faces to be rendered translucently",
        default=False,
        )
    
    global_scale = FloatProperty(
        name="Scale",
        min=0.01, max=1000.0,
        default=1.0,
        )
    
    
    # The actual export routine
    def execute(self, context):
        
        # Get file names for the output files
        filepath = self.filepath
        filepath = bpy.path.ensure_ext(filepath, self.filename_ext)
        PLYfilepath = bpy.path.ensure_ext(filepath, '.ply')
        MATfilepath = bpy.path.ensure_ext(filepath, '.mat')
        
        # Perform some pre-processing prior to exporting
        scene = context.scene
        obj   = context.active_object
        mesh  = obj.to_mesh(scene, self.apply_modifiers, 'PREVIEW') 
        
        if not mesh.tessfaces and mesh.polygons:
            mesh.calc_tessface()
        
        
        # Check if vertex colors and UV maps are present
        mesh_has_vcols = bool(mesh.tessface_vertex_colors)
        mesh_has_uvs   = bool(mesh.tessface_uv_textures)        
        
        # To simplify things later on
        if mesh_has_vcols:
            mesh_vcols = mesh.tessface_vertex_colors.active
            if mesh_vcols:
                mesh_vcols = mesh_vcols.data   
        else:
            mesh_vcols = None
        
        if mesh_has_uvs:
            mesh_uvs = mesh.tessface_uv_textures.active
            if mesh_uvs:
                mesh_uvs = mesh.tessface_uv_textures[0].data
        else:
            mesh_uvs = None
        
        mesh_verts = mesh.vertices
        mesh_polys = mesh.polygons

        
        # Check faces to see if there are non 3 or 4 point polygons
        for p in mesh_polys:
            if (len(p.vertices)<3) or (len(p.vertices)>4):
                raise Exception("Error, polygons must only have 3 or 4 points")

        
        # Copy normals into a simplified array for convenience
        if self.write_normals:
            mesh_norms = []
            for n in mesh_verts:
                addNorm = True
                
                # Optimize normals by eliminating duplicate entries
                if self.optimize_normals:
                    if len(mesh_norms)>0:
                        for ns in mesh_norms:
                           if (n.normal.x == ns.x) and (n.normal.y == ns.y) and (n.normal.z == ns.z):
                                addNorm = False
                                break
                
                if addNorm:
                    mesh_norms.append(n.normal)
        
        
        # Scan through all faces for their assigned textures
        if mesh_uvs:
            
            tex_table = []
            tex_list = []
        
            for uv in mesh_uvs:
                
                if uv.image is not None:
                    
                    addTex = True
                    tex_fileName = bpy.path.ensure_ext(bpy.path.basename(uv.image.filepath), '.tim')
                    
                    if len(tex_list)>0:
                        for c,t in enumerate(tex_list):
                            if t==tex_fileName:
                                tex_table.append(c+1)
                                addTex = False
                                break
                    
                    if addTex:
                        tex_list.append(tex_fileName)
                        tex_table.append(len(tex_list))
                        
                        
                else:
                    
                    tex_table.append(0)
                    
        else:
            
            tex_table = None
            tex_list = None
        
        
        # Prepare header comment strings
        rsd_header1 = "# Created by Blender %s - " \
            "www.blender.org, source file: %r\n" % \
            (bpy.app.version_string, os.path.basename(bpy.data.filepath))
        rsd_header2 = "# Exported using PlayStation RSD plug-in by " \
            "Jobert 'Lameguy' Villamor (Lameguy64/TheCodingBrony)\r\n"
        
        
        # Write PLY file (contains vertex, normal, and face indices)
        file = open(PLYfilepath, "w")
        fw = file.write
        
        # Write header
        fw("%s%s" % (rsd_header1, rsd_header2))
        fw("@PLY940102\n")
        fw("# Vertex count, normal count, polygon count\n")
        if self.write_normals:
            fw("%d %d %d\n" % (len(mesh_verts), len(mesh_norms), len(mesh_polys)))
        else:
            fw("%d 1 %d\n" % (len(mesh_verts), len(mesh_polys)))
        
        # Write vertex coords
        fw("# Vertices\n")
        for v in mesh_verts:
            fw("%E %E %E\n" % (v.co.x, -v.co.z, v.co.y))

        # Write normal coords
        fw("# Normals\n")
        if self.write_normals:
            for n in mesh_norms:
                fw("%E %E %E\n" % (n.x, -n.z, n.y))
        else:
            fw("%E %E %E\n" % (0, 0, 0))
        
        
        # Set global polygon flags
        poly_flags = 0
        
        if self.write_normals==False:   # No light source calc.
            poly_flags = 1
            
        if self.translucent_polys:      # Translucent
            poly_flags = poly_flags | (1<<2)
        
        
        # Write faces
        fw("# Polygons\n")
        for p in mesh_polys:
            
            normals_index = [ 0, 0, 0, 0 ]
            
            # Triangles
            if len(p.vertices)==3:
                
                fw("0 %d %d %d 0 " % (p.vertices[0], p.vertices[2], p.vertices[1]))
                
                if self.write_normals:
                    
                    if self.optimize_normals:
                        
                        for i,n in enumerate(mesh_norms):
                            if (mesh_verts[p.vertices[0]].normal.x == n.x and
                                mesh_verts[p.vertices[0]].normal.y == n.y and
                                mesh_verts[p.vertices[0]].normal.z == n.z):
                                normals_index[0] = i
                            if (mesh_verts[p.vertices[1]].normal.x == n.x and
                                mesh_verts[p.vertices[1]].normal.y == n.y and
                                mesh_verts[p.vertices[1]].normal.z == n.z):
                                normals_index[1] = i
                            if (mesh_verts[p.vertices[2]].normal.x == n.x and
                                mesh_verts[p.vertices[2]].normal.y == n.y and
                                mesh_verts[p.vertices[2]].normal.z == n.z):
                                normals_index[2] = i
                            i += 1
                    
                    else:
                        
                        normals_index[0] = p.vertices[0]
                        normals_index[1] = p.vertices[1]
                        normals_index[2] = p.vertices[2]
                        
                    fw("%d %d %d 0\n" % (normals_index[0], normals_index[2], normals_index[1]))
                    
                else:
                    
                    fw("0 0 0 0\n")
            
            # Quads
            else:
                
                fw("1 %d %d %d %d " % (p.vertices[3], p.vertices[2], p.vertices[0], p.vertices[1]))
                
                if self.write_normals:
                    
                    if self.optimize_normals:
                        
                        for i,n in enumerate(mesh_norms):
                            if (mesh_verts[p.vertices[0]].normal.x == n.x and
                                mesh_verts[p.vertices[0]].normal.y == n.y and
                                mesh_verts[p.vertices[0]].normal.z == n.z):
                                normals_index[0] = i
                            if (mesh_verts[p.vertices[1]].normal.x == n.x and
                                mesh_verts[p.vertices[1]].normal.y == n.y and
                                mesh_verts[p.vertices[1]].normal.z == n.z):
                                normals_index[1] = i
                            if (mesh_verts[p.vertices[2]].normal.x == n.x and
                                mesh_verts[p.vertices[2]].normal.y == n.y and
                                mesh_verts[p.vertices[2]].normal.z == n.z):
                                normals_index[2] = i
                            if (mesh_verts[p.vertices[3]].normal.x == n.x and
                                mesh_verts[p.vertices[3]].normal.y == n.y and
                                mesh_verts[p.vertices[3]].normal.z == n.z):
                                normals_index[3] = i
                            
                    else:
                        
                        normals_index[0] = p.vertices[0]
                        normals_index[1] = p.vertices[1]
                        normals_index[2] = p.vertices[2]
                        normals_index[3] = p.vertices[3]
                    
                    if p.use_smooth==False:
                        normals_index[1] = normals_index[0]
                        normals_index[2] = normals_index[0]
                        normals_index[3] = normals_index[0]
                        
                    fw("%d %d %d %d\n" % (normals_index[3], normals_index[2], normals_index[0], normals_index[1]))
                    
                else:
                
                    fw("0 0 0 0\n")
        
        file.close
        
        
        # Create MAT file
        file = open(MATfilepath, "w")
        fw = file.write
        
        # Write header
        fw("%s%s" % (rsd_header1, rsd_header2))
        fw("@MAT940801\n")
        fw("%d\n" % (len(mesh_polys)))
        
        for i,p in enumerate(mesh_polys):
            
            if (mesh_has_vcols):
                vc = mesh_vcols[i]
            
            vr = [ 0, 0, 0, 0 ]
            vg = [ 0, 0, 0, 0 ]
            vb = [ 0, 0, 0, 0 ]
            
            if p.use_smooth:
                shading = "G"
            else:
                shading = "F"
            
            
            # Polygon index, flags, and shading mode    
            t_poly_flags = poly_flags
            if (self.colored_tpolys):
                if (tex_table is not None):
                    if (tex_table[i]):
                        t_poly_flags = poly_flags | 1
            
            fw("%d\t %d %s " % (i, t_poly_flags, shading))
            
            
            poly_isGouraud = True
            
            if mesh_vcols:
                vr[0] = round(255*vc.color1.r)
                vg[0] = round(255*vc.color1.g)
                vb[0] = round(255*vc.color1.b)
                vr[1] = round(255*vc.color2.r)
                vg[1] = round(255*vc.color2.g)
                vb[1] = round(255*vc.color2.b)
                vr[2] = round(255*vc.color3.r)
                vg[2] = round(255*vc.color3.g)
                vb[2] = round(255*vc.color3.b)
                vr[3] = round(255*vc.color4.r)
                vg[3] = round(255*vc.color4.g)
                vb[3] = round(255*vc.color4.b)
            else:
                vr[0] = 255
                vg[0] = 255
                vb[0] = 255
                vr[1] = 255
                vg[1] = 255
                vb[1] = 255
                vr[2] = 255
                vg[2] = 255
                vb[2] = 255
                vr[3] = 255
                vg[3] = 255
                vb[3] = 255
            
            
            poly_isTextured = False
            if tex_table is not None:
                if tex_table[i]:
                    poly_isTextured = True
            
                    
            # Triangle
            if len(mesh_polys[i].vertices) == 3:
                
                if (vr[0] == vr[1] == vr[2]):
                    if (vg[0] == vg[1] == vg[2]):
                        if (vb[0] == vb[1] == vb[2]):
                            poly_isGouraud = False
                
                if poly_isTextured == False:
                
                    # Flat
                    if poly_isGouraud == False:
                        
                        fw("C ")
                        fw("%d %d %d " % (vr[0], vg[0], vb[0]))
                    
                    # Gouraud
                    else:
                        
                        fw("G ")
                        fw("%d %d %d " % (vr[0], vg[0], vb[0]))
                        fw("%d %d %d " % (vr[2], vg[2], vb[2]))
                        fw("%d %d %d " % (vr[1], vg[1], vb[1]))
                        fw("0 0 0")
                        
                else:
                    
                    # Textured
                    tex_width  = mesh_uvs[i].image.size[0]-1
                    tex_height = mesh_uvs[i].image.size[1]-1
                    
                    if self.colored_tpolys == False:
                        fw("T %d " % (tex_table[i]-1))
                        fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv1.x), round(float(tex_height)*(1-mesh_uvs[i].uv1.y))))
                        fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv3.x), round(float(tex_height)*(1-mesh_uvs[i].uv3.y))))
                        fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv2.x), round(float(tex_height)*(1-mesh_uvs[i].uv2.y))))
                        fw("0 0")
                    else:
                        if poly_isGouraud:  # Gouraud colored
                            fw("H %d " % (tex_table[i]-1))
                            fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv1.x), round(float(tex_height)*(1-mesh_uvs[i].uv1.y))))
                            fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv3.x), round(float(tex_height)*(1-mesh_uvs[i].uv3.y))))
                            fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv2.x), round(float(tex_height)*(1-mesh_uvs[i].uv2.y))))
                            fw("0 0 ")
                            fw("%d %d %d " % (vr[0]/2, vg[0]/2, vb[0]/2))
                            fw("%d %d %d " % (vr[2]/2, vg[2]/2, vb[2]/2))
                            fw("%d %d %d " % (vr[1]/2, vg[1]/2, vb[1]/2))
                            fw("0 0 0")
                        else:   # Flat colored
                            fw("D %d " % (tex_table[i]-1))
                            fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv1.x), round(float(tex_height)*(1-mesh_uvs[i].uv1.y))))
                            fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv3.x), round(float(tex_height)*(1-mesh_uvs[i].uv3.y))))
                            fw("%d %d " % (round(float(tex_width)*mesh_uvs[i].uv2.x), round(float(tex_height)*(1-mesh_uvs[i].uv2.y))))
                            fw("0 0 ")
                            fw("%d %d %d " % (vr[0]/2, vg[0]/2, vb[0]/2))
                            
            # Quad
            else:
                
                if (vr[0] == vr[1] == vr[2] == vr[3]):
                    if (vg[0] == vg[1] == vg[2] == vg[3]):
                        if (vb[0] == vb[1] == vb[2] == vb[3]):
                            poly_isGouraud = False
                
                if poly_isTextured == False:
                    
                    # Flat
                    if poly_isGouraud == False:
                        
                        fw("C ")
                        fw("%d %d %d " % (vr[0], vg[0], vb[0]))
                        
                    # Gouraud
                    else:
                        
                        fw("G ")
                        fw("%d %d %d " % (vr[3], vg[3], vb[3]))
                        fw("%d %d %d " % (vr[2], vg[2], vb[2]))
                        fw("%d %d %d " % (vr[0], vg[0], vb[0]))
                        fw("%d %d %d" % (vr[1], vg[1], vb[1]))
                    
                else:
                    
                    tex_width  = mesh_uvs[i].image.size[0]-1
                    tex_height = mesh_uvs[i].image.size[1]-1
                    
                    # WORK NEEDED:
                    # UV coords are sometimes output in reverse orientation (ANNOYING! Tricky to replicate too...)
                    
                    # Textured
                    if self.colored_tpolys == False:
                        fw("T %d " % (tex_table[i]-1))
                        fw("%d %d " % (round(tex_width*mesh_uvs[i].uv4.x), round(tex_height*(1-mesh_uvs[i].uv4.y))))
                        fw("%d %d " % (round(tex_width*mesh_uvs[i].uv3.x), round(tex_height*(1-mesh_uvs[i].uv3.y))))
                        fw("%d %d " % (round(tex_width*mesh_uvs[i].uv1.x), round(tex_height*(1-mesh_uvs[i].uv1.y))))
                        fw("%d %d" % (round(tex_width*mesh_uvs[i].uv2.x), round(tex_height*(1-mesh_uvs[i].uv2.y))))
                    else:
                        if poly_isGouraud:  # Gouraud colored
                            fw("H %d " % (tex_table[i]-1))
                            fw("%d %d " % (round(tex_width*mesh_uvs[i].uv4.x), round(tex_height*(1-mesh_uvs[i].uv4.y))))
                            fw("%d %d " % (round(tex_width*mesh_uvs[i].uv3.x), round(tex_height*(1-mesh_uvs[i].uv3.y))))
                            fw("%d %d " % (round(tex_width*mesh_uvs[i].uv1.x), round(tex_height*(1-mesh_uvs[i].uv1.y))))
                            fw("%d %d " % (round(tex_width*mesh_uvs[i].uv2.x), round(tex_height*(1-mesh_uvs[i].uv2.y))))
                            fw("%d %d %d " % (vr[3]/2, vg[3]/2, vb[3]/2))
                            fw("%d %d %d " % (vr[2]/2, vg[2]/2, vb[2]/2))
                            fw("%d %d %d " % (vr[0]/2, vg[0]/2, vb[0]/2))
                            fw("%d %d %d" % (vr[1]/2, vg[1]/2, vb[1]/2))
                        else:   # Flat colored
                            fw("D %d " % (tex_table[i]-1))
                            fw("%d %d " % (round(tex_width*mesh_uvs[i].uv4.x), round(tex_height*(1-mesh_uvs[i].uv4.y))))
                            fw("%d %d " % (round(tex_width*mesh_uvs[i].uv3.x), round(tex_height*(1-mesh_uvs[i].uv3.y))))
                            fw("%d %d " % (round(tex_width*mesh_uvs[i].uv1.x), round(tex_height*(1-mesh_uvs[i].uv1.y))))
                            fw("%d %d " % (round(tex_width*mesh_uvs[i].uv2.x), round(tex_height*(1-mesh_uvs[i].uv2.y))))
                            fw("%d %d %d " % (vr[0]/2, vg[0]/2, vb[0]/2))
                    
            fw("\n")
            
        file.close
        
		
        # Create RSD file
        file = open(filepath, "w")
        fw = file.write
        
        fw("%s%s" % (rsd_header1, rsd_header2))
        fw("@RSD940102\n")
        
        fw("PLY=%s\n" % os.path.basename(PLYfilepath))
        fw("MAT=%s\n" % os.path.basename(MATfilepath))
        
        if tex_list is not None:
            fw("NTEX=%d\n" % (len(tex_list)))
            for i,t in enumerate(tex_list):
                fw('TEX[%d]=%s\n' % (i, t))
        else:
            fw("NTEX=0\n")
        
        file.close
        
        
        # Finish
        return {'FINISHED'};

    
# For registering to Blender menus
def menu_func(self, context):
    self.layout.operator(ExportRSD.bl_idname, text="PlayStation RSD (.rsd,.ply,.mat)");

def register():
    bpy.utils.register_module(__name__);
    bpy.types.INFO_MT_file_export.append(menu_func);
    
def unregister():
    bpy.utils.unregister_module(__name__);
    bpy.types.INFO_MT_file_export.remove(menu_func);

if __name__ == "__main__":
    register()
