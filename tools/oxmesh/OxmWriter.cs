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

            int nPolys = 0;
            foreach (Attribute attr in oxm.attribTable) nPolys += attr.polygons.Count;
            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: OXM data constructed: {0} polygons, over {1} attributes.", nPolys, oxm.attribTable.Count);

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

            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: Writing OXM file with scale factor {0}.", scaleFactor);

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
                    writer.Write(texName.ToCharArray());
                    nUnpadded += texName.Length + 1;
                }

                // Pad to 32-bit boundary
                for (int i = 0; i < 4 - (nUnpadded % 4); i++) writer.Write((byte)0);

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
                    foreach (Polygon poly in attr.polygons)
                    {
                        WriteOxmVertex(poly.v1, attr, true, scaleFactor, writer);
                        WriteOxmVertex(poly.v2, attr, false, scaleFactor, writer);
                        WriteOxmVertex(poly.v3, attr, false, scaleFactor, writer);
                        if (attr.IsQuad()) WriteOxmVertex(poly.v4, attr, false, scaleFactor, writer);
                    }
                }
            }

            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: OXM file successfully written.", scaleFactor);
        }

        private void WriteOxmVertex(Vertex vert, Attribute attr, bool bFirstVert, int scaleFactor, BinaryWriter writer)
        {
            writer.Write((short)(vert.pos.x * scaleFactor));
            writer.Write((short)(vert.pos.y * scaleFactor));
            writer.Write((short)(vert.pos.z * scaleFactor));
            writer.Write((short)0); // Padding

            // For flat lighting, only write the first normal
            if (attr.lightType != Attribute.LightingType.None
                && (bFirstVert || attr.lightType == Attribute.LightingType.Smooth))
            {
                writer.Write(EncodeGteFloat(vert.normal.x));
                writer.Write(EncodeGteFloat(vert.normal.y));
                writer.Write(EncodeGteFloat(vert.normal.z));
                writer.Write((short)0); // Padding
            }

            // For flat coloring, only write the first color
            if (bFirstVert || attr.IsGouraud())
            {
                writer.Write((byte)vert.color.r);
                writer.Write((byte)vert.color.g);
                writer.Write((byte)vert.color.b);
                writer.Write((byte)0); // Padding
            }

            if (attr.IsTextured())
            {
                writer.Write((byte)vert.uv.x);
                writer.Write((byte)vert.uv.y);
                writer.Write((short)0); // Padding
            }
        }

        // 1 sign bit, 3 whole bits, 12 fractional bits
        private short EncodeGteFloat(float val)
        {
            return (short)(val * (1 << 12));
        }
    }
}
