using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace OxMesh
{
    class OxmWriter
    {
        public class WriteException : Exception { public WriteException(string message) : base(message) {} }
        public class ConstructException : Exception { public ConstructException(string message) : base(message) { } }

        private RsdFilesetParser.Parsed parsedRsd;
        private OxmContents oxmContents;

        public OxmWriter(RsdFilesetParser.Parsed parsedRsd)
        {
            this.parsedRsd = parsedRsd;
            this.oxmContents = ConstructOxm();
        }

        public void WriteToFile(string outFilePath, int scaleFactor)
        {
            oxmContents.scaleFactor = scaleFactor;
            WriteOxm(outFilePath);
        }

        private struct Vertex
        {
            public Vector3 pos;
            public Vector3 normal;
            public RgbColor color;
            public Vector2 uv;
        }

        private struct Polygon
        {
            public Vertex v1;
            public Vertex v2;
            public Vertex v3;
            public Vertex v4;
        }

        private class Attribute
        {
            public enum LightingType { None, Flat, Smooth }
            public enum PolygonType
            {
                PolyF3, PolyF4,
                PolyG3, PolyG4,
                PolyFT3, PolyFT4,
                PolyGT3, PolyGT4
            }

            public int textureIndex;
            public PolygonType polyType;
            public LightingType lightType;
            public bool bDoubleSided;
            public int semiTransparentCode; // -1 == not semitransparent

            public List<Polygon> polygons = new List<Polygon>();

            public bool IsGouraud()
            {
                switch (polyType)
                {
                    case PolygonType.PolyG3:
                    case PolygonType.PolyG4:
                    case PolygonType.PolyGT3:
                    case PolygonType.PolyGT4:
                        return true;
                    default:
                        return false;
                }
            }

            public bool IsTextured()
            {
                switch (polyType)
                {
                    case PolygonType.PolyFT3:
                    case PolygonType.PolyFT4:
                    case PolygonType.PolyGT3:
                    case PolygonType.PolyGT4:
                        return true;
                    default:
                        return false;
                }
            }

            public bool IsQuad()
            {
                switch (polyType)
                {
                    case PolygonType.PolyF4:
                    case PolygonType.PolyFT4:
                    case PolygonType.PolyG4:
                    case PolygonType.PolyGT4:
                        return true;
                    default:
                        return false;
                }
            }
        }

        private class OxmContents
        {
            public int scaleFactor = 1;
            public List<string> textureNames = new List<string>();
            public List<Attribute> attribTable = new List<Attribute>();
            public Vector3 aabbMin;
            public Vector3 aabbMax;
        }

        private OxmContents ConstructOxm()
        {
            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: Constructing OXM data from the parsed RSD fileset.");

            OxmContents oxm = new OxmContents();
            oxm.textureNames = new List<string>(parsedRsd.rsd.texFiles);

            foreach (RsdFilesetParser.MatContents.MatDesc mat in parsedRsd.mat.materials)
            {
                // Retrieve and translate polygon and material information
                Attribute.LightingType lightType = TranslateLightType(mat);
                RsdFilesetParser.PlyContents.Polygon plyPoly = parsedRsd.ply.polys[mat.polyIdx];
                int semiCode = mat.bSemiTransparent ? mat.semiTransparentCode : -1;
                Attribute.PolygonType polyType;
                Attribute attr = null;
                bool bQuad;

                switch (plyPoly.polyType)
                {
                    case RsdFilesetParser.PlyContents.PolyType.Quad:
                        bQuad = true;
                        polyType = TranslatePolyType(mat, bQuad);
                        break;
                    case RsdFilesetParser.PlyContents.PolyType.Tri:
                        bQuad = false;
                        polyType = TranslatePolyType(mat, bQuad);
                        break;
                    case RsdFilesetParser.PlyContents.PolyType.Line:
                    case RsdFilesetParser.PlyContents.PolyType.Sprite:
                        throw new ConstructException("Unsupported PLY polygon type");
                    default:
                        throw new ConstructException("Unrecognized PLY polygon type");
                }
                
                // Find a matching attribute
                int i;
                for (i = 0; i < oxm.attribTable.Count; i++)
                {
                    attr = oxm.attribTable[i];
                    if (attr.bDoubleSided == mat.bDoubleSided
                        && attr.semiTransparentCode == semiCode
                        && attr.lightType == lightType
                        && attr.polyType == polyType
                        && attr.textureIndex == mat.texIdx) break;
                }

                // Create one if not found
                if (i == oxm.attribTable.Count)
                {
                    attr = new Attribute();
                    attr.bDoubleSided = mat.bDoubleSided;
                    attr.semiTransparentCode = semiCode;
                    attr.polyType = polyType;
                    attr.lightType = lightType;
                    attr.textureIndex = mat.texIdx;
                    oxm.attribTable.Add(attr);
                }

                // Copy over positions and normals into an OXM polygon
                Polygon oxmPoly = new Polygon();
                oxmPoly.v1.pos = parsedRsd.ply.positions[plyPoly.posIndices[0]];
                oxmPoly.v1.normal = parsedRsd.ply.normals[plyPoly.normIndices[0]];
                oxmPoly.v2.pos = parsedRsd.ply.positions[plyPoly.posIndices[1]];
                oxmPoly.v2.normal = parsedRsd.ply.normals[plyPoly.normIndices[1]];
                oxmPoly.v3.pos = parsedRsd.ply.positions[plyPoly.posIndices[2]];
                oxmPoly.v3.normal = parsedRsd.ply.normals[plyPoly.normIndices[2]];
                if (bQuad)
                {
                    oxmPoly.v4.pos = parsedRsd.ply.positions[plyPoly.posIndices[3]];
                    oxmPoly.v4.normal = parsedRsd.ply.normals[plyPoly.normIndices[3]];
                }

                // Copy over colors
                if (attr.IsGouraud())
                {
                    oxmPoly.v1.color = mat.vertColors[0];
                    oxmPoly.v2.color = mat.vertColors[1];
                    oxmPoly.v3.color = mat.vertColors[2];
                    if (bQuad) oxmPoly.v4.color = mat.vertColors[3];
                }
                else
                {
                    RgbColor color;
                    if (mat.matType == RsdFilesetParser.MatContents.MaterialType.TexturedUncolored) color = new RgbColor(128, 128, 128);
                    else color = mat.vertColors[0];
                    oxmPoly.v1.color = color;
                    oxmPoly.v2.color = color;
                    oxmPoly.v3.color = color;
                    if (bQuad) oxmPoly.v4.color = color;
                }

                // Copy over UV coords
                if (attr.IsTextured())
                {
                    oxmPoly.v1.uv = mat.vertUvs[0];
                    oxmPoly.v2.uv = mat.vertUvs[1];
                    oxmPoly.v3.uv = mat.vertUvs[2];
                    if (bQuad) oxmPoly.v4.uv = mat.vertUvs[3];
                }

             
                attr.polygons.Add(oxmPoly);
            }

            // Min/max bounds
            Vector3 min = new Vector3(float.MaxValue, float.MaxValue, float.MaxValue);
            Vector3 max = new Vector3(-float.MaxValue, -float.MaxValue, -float.MaxValue);
            foreach (Attribute attr in oxm.attribTable)
            {
                foreach (Polygon poly in attr.polygons)
                {
                    Vertex[] verts = new Vertex[] { poly.v1, poly.v2, poly.v3, poly.v4 };
                    for (int i = 0; i < (attr.IsQuad() ? 4 : 3); i++)
                    {
                        Vertex vert = verts[i];
                        min.x = Math.Min(min.x, vert.pos.x);
                        min.y = Math.Min(min.y, vert.pos.y);
                        min.z = Math.Min(min.z, vert.pos.z);
                        max.x = Math.Max(max.x, vert.pos.x);
                        max.y = Math.Max(max.y, vert.pos.y);
                        max.z = Math.Max(max.z, vert.pos.z);
                    }
                }
            }
            oxm.aabbMin = min;
            oxm.aabbMax = max;

            if (!ProgramArgs.inst.bQuiet)
            {
                int nPolys = 0;
                foreach (Attribute attr in oxm.attribTable) nPolys += attr.polygons.Count;
                Console.WriteLine("INFO: OXM data constructed: {0} polygons, over {1} attributes.", nPolys, oxm.attribTable.Count);
                Console.WriteLine("INFO: Unscaled AABB: [{0}, {1}, {2}] -> [{3}, {4}, {5}]", min.x, min.y, min.z, max.x, max.y, max.z);
            }

            return oxm;
        }

        private Attribute.LightingType TranslateLightType(RsdFilesetParser.MatContents.MatDesc mat)
        {
            // Translate the lighting type
            if (mat.bLit)
            {
                if (mat.lightSmoothing == RsdFilesetParser.MatContents.LightSmoothing.Flat)
                    return Attribute.LightingType.Flat;
                else if (mat.lightSmoothing == RsdFilesetParser.MatContents.LightSmoothing.Smooth)
                    return Attribute.LightingType.Smooth;
                else throw new ConstructException("Unrecognized MAT light smoothing type");
            }
            else return Attribute.LightingType.None;
        }

        private Attribute.PolygonType TranslatePolyType(RsdFilesetParser.MatContents.MatDesc mat, bool bQuad)
        {
            switch (mat.matType)
            {
                case RsdFilesetParser.MatContents.MaterialType.Flat:
                    return bQuad ? Attribute.PolygonType.PolyF4 : Attribute.PolygonType.PolyF3;
                case RsdFilesetParser.MatContents.MaterialType.Gouraud:
                    return bQuad ? Attribute.PolygonType.PolyG4 : Attribute.PolygonType.PolyG3;
                case RsdFilesetParser.MatContents.MaterialType.TexturedFlat:
                case RsdFilesetParser.MatContents.MaterialType.TexturedUncolored:
                    return bQuad ? Attribute.PolygonType.PolyFT4 : Attribute.PolygonType.PolyFT3;
                case RsdFilesetParser.MatContents.MaterialType.TexturedGouraud:
                    return bQuad ? Attribute.PolygonType.PolyGT4 : Attribute.PolygonType.PolyGT3;
                default:
                    throw new ConstructException("Unrecognized MAT material type");
            }
        }

        private void WriteOxm(string outFilePath)
        {
            int version = 1;
            int scaleFactor = oxmContents.scaleFactor;

            if (!ProgramArgs.inst.bQuiet)
            {
                Console.WriteLine("INFO: Writing OXM file with scale factor {0}.", scaleFactor);

                Vector3 min = oxmContents.aabbMin;
                Vector3 max = oxmContents.aabbMax;
                min.x = (float)((short)(min.x * scaleFactor));
                min.y = (float)((short)(min.y * scaleFactor));
                min.z = (float)((short)(min.z * scaleFactor));
                max.x = (float)((short)(max.x * scaleFactor));
                max.y = (float)((short)(max.y * scaleFactor));
                max.z = (float)((short)(max.z * scaleFactor));
                Console.WriteLine("INFO: Scaled AABB: [{0}, {1}, {2}] -> [{3}, {4}, {5}]", min.x, min.y, min.z, max.x, max.y, max.z);
            }

            

            // BinaryWriter's docs says it uses little-endian, so we
            // don't have to do anything special for that
            using (BinaryWriter writer = new BinaryWriter(new FileStream(outFilePath, FileMode.Create), Encoding.ASCII))
            {
                writer.Write(version);
                
                // Header
                writer.Write(scaleFactor);
                
                // Texture name table
                int nUnpadded = 1;
                writer.Write((byte)oxmContents.textureNames.Count);
                foreach (string texName in oxmContents.textureNames)
                {
                    // We write out strings manually to be more compact
                    writer.Write((byte)texName.Length);
                    writer.Write(texName.ToUpper().ToCharArray());
                    nUnpadded += texName.Length + 1;
                }

                // Pad to 32-bit boundary
                for (int i = 0; i < (4 - (nUnpadded % 4)) % 4; i++) writer.Write((byte)0);

                // Attribute table (minus the polygons)
                writer.Write(oxmContents.attribTable.Count);
                foreach (Attribute attr in oxmContents.attribTable)
                {
                    byte flags = 0;
                    if (attr.bDoubleSided) flags |= 0x1;

                    writer.Write(attr.polygons.Count);

                    writer.Write((byte)attr.polyType);
                    writer.Write((byte)attr.lightType);
                    writer.Write((byte)attr.semiTransparentCode);
                    writer.Write((byte)attr.textureIndex);
                    writer.Write(flags);

                    // Pad to 32-bit boundary
                    writer.Write((byte)0);
                    writer.Write((byte)0);
                    writer.Write((byte)0);
                }

                // The polys from each attribute, all concatenated together
                foreach (Attribute attr in oxmContents.attribTable)
                {
                    foreach (Polygon poly in attr.polygons) WriteOxmPolygon(attr, poly, scaleFactor, writer);
                }
            }

            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: OXM file successfully written.", scaleFactor);
        }

        private void WriteOxmPolygon(Attribute attr, Polygon poly, int scaleFactor, BinaryWriter writer)
        {
            // Color
            WriteOxmColor(poly.v1.color, writer);
            if (attr.IsGouraud())
            {
                WriteOxmColor(poly.v2.color, writer);
                WriteOxmColor(poly.v3.color, writer);
                if (attr.IsQuad()) WriteOxmColor(poly.v4.color, writer);
            }

            // Position
            WriteOxmPosition(poly.v1.pos, scaleFactor, writer);
            WriteOxmPosition(poly.v2.pos, scaleFactor, writer);
            WriteOxmPosition(poly.v3.pos, scaleFactor, writer);
            if (attr.IsQuad()) WriteOxmPosition(poly.v4.pos, scaleFactor, writer);

            // Tex coords
            if (attr.IsTextured())
            {
                writer.Write((byte)poly.v1.uv.x);
                writer.Write((byte)poly.v1.uv.y);
                writer.Write((byte)poly.v2.uv.x);
                writer.Write((byte)poly.v2.uv.y);
                writer.Write((byte)poly.v3.uv.x);
                writer.Write((byte)poly.v3.uv.y);
                if (attr.IsQuad())
                {
                    writer.Write((byte)poly.v4.uv.x);
                    writer.Write((byte)poly.v4.uv.y);
                }
                else
                {
                    writer.Write((byte)0);
                    writer.Write((byte)0);
                }
            }

            // Normals
            if (attr.lightType != Attribute.LightingType.None)
            {
                WriteOxmNormal(poly.v1.normal, writer);
                if (attr.lightType == Attribute.LightingType.Smooth)
                {
                    WriteOxmNormal(poly.v2.normal, writer);
                    WriteOxmNormal(poly.v3.normal, writer);
                    if (attr.IsQuad()) WriteOxmNormal(poly.v4.normal, writer);
                }
            }
        }

        private void WriteOxmColor(RgbColor color, BinaryWriter writer)
        {
            writer.Write((byte)color.r);
            writer.Write((byte)color.g);
            writer.Write((byte)color.b);
            writer.Write((byte)0); // Padding
        }

        private void WriteOxmPosition(Vector3 pos, int scaleFactor, BinaryWriter writer)
        {
            writer.Write((short)(pos.x * scaleFactor));
            writer.Write((short)(pos.y * scaleFactor));
            writer.Write((short)(pos.z * scaleFactor));
            writer.Write((short)0); // Padding
        }

        private void WriteOxmNormal(Vector3 normal, BinaryWriter writer)
        {
            writer.Write(EncodeGteFloat(normal.x));
            writer.Write(EncodeGteFloat(normal.y));
            writer.Write(EncodeGteFloat(normal.z));
            writer.Write((short)0); // Padding
        }

        // 1 sign bit, 3 whole bits, 12 fractional bits
        private short EncodeGteFloat(float val)
        {
            return (short)(val * (1 << 12));
        }
    }
}
