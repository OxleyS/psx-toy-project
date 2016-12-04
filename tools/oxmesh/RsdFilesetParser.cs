using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace OxMesh
{
    class RsdFilesetParser
    {
        public class ParseException : Exception
        {
            public string fileLine;

            public ParseException(string message) : base(message)
            {
                this.fileLine = null;
            }

            public ParseException(string message, string fileLine) : base(message)
            {
                this.fileLine = fileLine;
            }
        }

        public class Parsed
        {
            public RsdContents rsd;
            public PlyContents ply;
            public MatContents mat;
        };

        public class RsdContents
        {
            public string plyPath;
            public string matPath;
            public string[] texFiles;
        }

        public class PlyContents
        {
            public enum PolyType { Tri, Quad, Line, Sprite }
            public struct Polygon
            {
                public PolyType polyType;
                public int[] posIndices;
                public int[] normIndices;
            }

            public Vector3[] positions;
            public Vector3[] normals;
            public Polygon[] polys;
        }

        public class MatContents
        {
            public enum LightSmoothing { Flat, Smooth }
            public enum MaterialType
            {
                Flat,
                Gouraud,
                TexturedUncolored,
                TexturedFlat,
                TexturedGouraud,
                WrapTexturedUncolored,
                WrapTexturedFlat,
                WrapTexturedGouraud,
            }
            public struct MatDesc
            {
                public int polyIdx;
                public MaterialType matType;
                public int texIdx;
                public bool bLit;
                public bool bDoubleSided;
                public bool bHmdGeneration;
                public bool bSemiTransparent;
                public int semiTransparentCode;
                public LightSmoothing lightSmoothing;
                public RgbColor[] vertColors;
                public Vector2[] vertUvs;
            }

            public MatDesc[] materials;
        }

        public Parsed ParseRsdFileset(string rsdFilename)
        {
            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: Beginning parsing of RSD fileset.");

            Parsed parsed = new Parsed();
            parsed.rsd = ParseRsd(rsdFilename);

            if (!string.IsNullOrWhiteSpace(parsed.rsd.plyPath))
                parsed.ply = ParsePly(parsed.rsd.plyPath);
            if (!string.IsNullOrWhiteSpace(parsed.rsd.matPath))
                parsed.mat = ParseMat(parsed.rsd.matPath);

            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: Entire RSD fileset parsed successfully.");
            return parsed;
        }

        RsdContents ParseRsd(string filename)
        {
            RsdContents rsd = new RsdContents();
            int nTextures = -1;
            bool bVersionSeen = false;

            using (StreamReader reader = new StreamReader(filename))
            {
                string relativeBase = System.IO.Path.GetDirectoryName(filename);

                string line;
                while ((line = reader.ReadLine()) != null)
                {
                    line = line.Trim();
                    if (line.Length == 0 || line[0] == '#') continue; // Blanks, comments

                    if (line[0] == '@')
                    {
                        if (bVersionSeen) throw new ParseException("Duplicate version descriptor in RSD file", line);
                        if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: Parsing RSD Version " + line.Substring(1));
                        bVersionSeen = true;
                        continue;
                    }

                    int kvIndex = line.IndexOf('=');
                    if (kvIndex == -1) throw new ParseException("Unexpected line in RSD file", line);

                    string key = line.Substring(0, kvIndex).Trim();
                    string value = kvIndex != (line.Length - 1) ? line.Substring(kvIndex + 1) : "";
                    value = value.Trim();
                    if (value == "") continue;

                    if (string.Equals(key, "PLY", StringComparison.InvariantCultureIgnoreCase))
                        rsd.plyPath = ResolvePath(value, relativeBase);
                    else if (string.Equals(key, "MAT", StringComparison.InvariantCultureIgnoreCase))
                        rsd.matPath = ResolvePath(value, relativeBase);
                    else if (string.Equals(key, "NTEX", StringComparison.InvariantCultureIgnoreCase))
                    {
                        if (nTextures != -1) throw new ParseException("Duplicate NTEX line in RSD file", line);
                        nTextures = ParseInt(value, line);
                        rsd.texFiles = new string[nTextures];
                        for (int i = 0; i < rsd.texFiles.Length; i++) rsd.texFiles[i] = "";
                    }
                    else if (key.StartsWith("TEX[", StringComparison.InvariantCultureIgnoreCase) && key.EndsWith("]"))
                    {
                        if (nTextures == -1) throw new ParseException("TEX line encountered before NTEX line in RSD file", line);

                        string indexStr = key.Substring(4, key.Length - 5);
                        int index = ParseInt(indexStr, line);
                        if (index > nTextures - 1) throw new ParseException("Out of range texture index in RSD file", line);
                        rsd.texFiles[index] = value;
                    }
                    else throw new ParseException("Unrecognized key in RSD file", line);
                }
            }

            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: RSD parsed successfully.");
            return rsd;
        }

        PlyContents ParsePly(string filename)
        {
            PlyContents ply = new PlyContents();
            int parseState = 0; // Headers
            bool bCountsSeen = false;
            int nLinesThisState = 0;
            int nPositionsSeen = 0;
            int nNormalsSeen = 0;
            int nPolysSeen = 0;
            string[] splitStrs;
            Vector3 vec3;
            PlyContents.Polygon poly;
            bool bVersionSeen = false;

            using (StreamReader reader = new StreamReader(filename))
            {
                string line;
                while ((line = reader.ReadLine()) != null)
                {
                    line = line.Trim();

                    if (line.Length == 0 || line[0] == '#') // Blanks, comments
                    {
                        if (nLinesThisState > 0)
                        {
                            parseState++;
                            nLinesThisState = 0;
                        }
                        continue;
                    }

                    if (line[0] == '@')
                    {
                        if (bVersionSeen) throw new ParseException("Duplicate version descriptor in PLY file", line);
                        if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: Parsing PLY Version " + line.Substring(1));
                        bVersionSeen = true;
                        continue;
                    }

                    nLinesThisState++;
                    switch (parseState)
                    {
                        case 0: // Headers
                            if (bCountsSeen) throw new ParseException("Duplicate count header in PLY file", line);
                            splitStrs = line.Split(' ');
                            if (splitStrs.Length != 3) throw new ParseException("Bad count header in PLY file", line);
                            ply.positions = new Vector3[ParseInt(splitStrs[0], line)];
                            ply.normals = new Vector3[ParseInt(splitStrs[1], line)];
                            ply.polys = new PlyContents.Polygon[ParseInt(splitStrs[2], line)];
                            bCountsSeen = true;
                            break;
                        case 1: // Positions
                            if (nLinesThisState > ply.positions.Length) throw new ParseException("Too many position lines in PLY file", line);
                            splitStrs = line.Split(' ');
                            if (splitStrs.Length != 3) throw new ParseException("Bad position line in PLY file", line);

                            vec3 = new Vector3();
                            vec3.x = ParseFloat(splitStrs[0], line);
                            vec3.y = ParseFloat(splitStrs[1], line);
                            vec3.z = ParseFloat(splitStrs[2], line);
                            ply.positions[nLinesThisState - 1] = vec3;
                            nPositionsSeen++;

                            break;
                        case 2: // Normals
                            if (nLinesThisState > ply.normals.Length) throw new ParseException("Too many normal lines in PLY file", line);
                            splitStrs = line.Split(' ');
                            if (splitStrs.Length != 3) throw new ParseException("Bad normal line in PLY file", line);

                            vec3 = new Vector3();
                            vec3.x = ParseFloat(splitStrs[0], line);
                            vec3.y = ParseFloat(splitStrs[1], line);
                            vec3.z = ParseFloat(splitStrs[2], line);
                            ply.normals[nLinesThisState - 1] = vec3;
                            nNormalsSeen++;

                            break;
                        case 3: // Polygons
                            if (nLinesThisState > ply.polys.Length) throw new ParseException("Too many polygon lines in PLY file", line);
                            splitStrs = line.Split(' ');
                            if (splitStrs.Length != 9) throw new ParseException("Bad polygon line in PLY file", line);

                            poly = new PlyContents.Polygon();
                            poly.polyType = ParseEnumInt<PlyContents.PolyType>(splitStrs[0], line);
                            poly.posIndices = new int[4];
                            poly.normIndices = new int[4];
                            for (int i = 0; i < 4; i++) poly.posIndices[i] = ParseInt(splitStrs[1 + i], line);
                            for (int i = 0; i < 4; i++) poly.normIndices[i] = ParseInt(splitStrs[5 + i], line);
                            ply.polys[nLinesThisState - 1] = poly;
                            nPolysSeen++;

                            break;
                    }
                }
            }

            if (nPositionsSeen < ply.positions.Length) throw new ParseException("Missing positions in PLY file");
            if (nNormalsSeen < ply.normals.Length) throw new ParseException("Missing normals in PLY file");
            if (nPolysSeen < ply.polys.Length) throw new ParseException("Missing polygons in PLY file");

            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: PLY parsed successfully.");
            return ply;
        }

        MatContents ParseMat(string filename)
        {
            MatContents mat = new MatContents();
            int nDescLines = -1;
            int nSeenLines = 0;
            bool bVersionSeen = false;
            List<MatContents.MatDesc> matDescriptors = new List<MatContents.MatDesc>();
            List<int> polyIndices = new List<int>();

            Dictionary<string, MatContents.MaterialType> typeTable = new Dictionary<string, MatContents.MaterialType>(StringComparer.OrdinalIgnoreCase)
            {
                { "C", MatContents.MaterialType.Flat },
                { "G", MatContents.MaterialType.Gouraud },
                { "T", MatContents.MaterialType.TexturedUncolored },
                { "D", MatContents.MaterialType.TexturedFlat },
                { "H", MatContents.MaterialType.TexturedGouraud },
                { "W", MatContents.MaterialType.WrapTexturedUncolored },
                { "S", MatContents.MaterialType.WrapTexturedFlat },
                { "N", MatContents.MaterialType.WrapTexturedGouraud }
            };

            using (StreamReader reader = new StreamReader(filename))
            {
                string line;
                while ((line = reader.ReadLine()) != null)
                {
                    line = line.Trim();
                    if (line.Length == 0 || line[0] == '#') continue; // Blanks, comments

                    if (line[0] == '@')
                    {
                        if (bVersionSeen) throw new ParseException("Duplicate version descriptor in MAT file", line);
                        if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: Parsing MAT Version " + line.Substring(1));
                        bVersionSeen = true;
                        continue;
                    }

                    if (nDescLines == -1)
                    {
                        nDescLines = ParseInt(line, line);
                        continue;
                    }

                    nSeenLines++;
                    if (nSeenLines > nDescLines) throw new ParseException("Unexpected descriptor line in MAT file", line);

                    MatContents.MatDesc desc = new MatContents.MatDesc();
                    string[] splitVals = line.Split(' ');
                    if (splitVals.Length < 4) throw new ParseException("Malformed material descriptor in MAT file", line);

                    polyIndices.Clear();
                    string[] polyRangeSets = splitVals[0].Split(',');
                    foreach (string polyRange in polyRangeSets)
                    {
                        int rangeDelimIdx = polyRange.IndexOf('-');
                        if (rangeDelimIdx != -1)
                        {
                            int start = ParseInt(polyRange.Substring(0, rangeDelimIdx), line);
                            int end = ParseInt(polyRange.Substring(rangeDelimIdx + 1), line);
                            for (int i = start; i <= end; i++) polyIndices.Add(i);
                        }
                        else polyIndices.Add(ParseInt(polyRange, line));
                    }

                    uint flags = ParseUnprefixedHex(splitVals[1], line);
                    desc.bLit = (flags & 0x1) != 0;
                    desc.bDoubleSided = (flags & 0x2) != 0;
                    desc.bSemiTransparent = (flags & 0x4) != 0;
                    desc.semiTransparentCode = (int)((flags & 0x18) >> 3);
                    desc.bHmdGeneration = (flags & 0x80) != 0;

                    if (string.Equals(splitVals[2], "F", StringComparison.InvariantCultureIgnoreCase))
                        desc.lightSmoothing = MatContents.LightSmoothing.Flat;
                    else if (string.Equals(splitVals[2], "G", StringComparison.InvariantCultureIgnoreCase))
                        desc.lightSmoothing = MatContents.LightSmoothing.Smooth;
                    else throw new ParseException("Invalid shading mode", line);

                    if (!typeTable.TryGetValue(splitVals[3], out desc.matType))
                        throw new ParseException("Invalid material type", line);

                    switch (desc.matType)
                    {
                        case MatContents.MaterialType.Flat:
                            desc.texIdx = -1;
                            desc.vertUvs = new Vector2[0];
                            desc.vertColors = new RgbColor[1];
                            desc.vertColors[0] = PartialParseRgb(splitVals, 4, line);
                            break;
                        case MatContents.MaterialType.Gouraud:
                            desc.texIdx = -1;
                            desc.vertUvs = new Vector2[0];
                            desc.vertColors = new RgbColor[4];
                            desc.vertColors[0] = PartialParseRgb(splitVals, 4, line);
                            desc.vertColors[1] = PartialParseRgb(splitVals, 7, line);
                            desc.vertColors[2] = PartialParseRgb(splitVals, 10, line);
                            desc.vertColors[3] = PartialParseRgb(splitVals, 13, line);
                            break;
                        case MatContents.MaterialType.TexturedUncolored:
                            desc.texIdx = ParseInt(splitVals[4], line);
                            desc.vertUvs = new Vector2[4];
                            desc.vertUvs[0] = PartialParseUv(splitVals, 5, line);
                            desc.vertUvs[1] = PartialParseUv(splitVals, 7, line);
                            desc.vertUvs[2] = PartialParseUv(splitVals, 9, line);
                            desc.vertUvs[3] = PartialParseUv(splitVals, 11, line);
                            desc.vertColors = new RgbColor[0];
                            break;
                        case MatContents.MaterialType.TexturedFlat:
                            desc.texIdx = ParseInt(splitVals[4], line);
                            desc.vertUvs = new Vector2[4];
                            desc.vertUvs[0] = PartialParseUv(splitVals, 5, line);
                            desc.vertUvs[1] = PartialParseUv(splitVals, 7, line);
                            desc.vertUvs[2] = PartialParseUv(splitVals, 9, line);
                            desc.vertUvs[3] = PartialParseUv(splitVals, 11, line);
                            desc.vertColors = new RgbColor[1];
                            desc.vertColors[0] = PartialParseRgb(splitVals, 13, line);
                            break;
                        case MatContents.MaterialType.TexturedGouraud:
                            desc.texIdx = ParseInt(splitVals[4], line);
                            desc.vertUvs = new Vector2[4];
                            desc.vertUvs[0] = PartialParseUv(splitVals, 5, line);
                            desc.vertUvs[1] = PartialParseUv(splitVals, 7, line);
                            desc.vertUvs[2] = PartialParseUv(splitVals, 9, line);
                            desc.vertUvs[3] = PartialParseUv(splitVals, 11, line);
                            desc.vertColors = new RgbColor[4];
                            desc.vertColors[0] = PartialParseRgb(splitVals, 13, line);
                            desc.vertColors[1] = PartialParseRgb(splitVals, 16, line);
                            desc.vertColors[2] = PartialParseRgb(splitVals, 19, line);
                            desc.vertColors[3] = PartialParseRgb(splitVals, 22, line);
                            break;
                        default:
                            throw new ParseException("Unsupported material type", line);
                    }

                    foreach (int polyIdx in polyIndices)
                    {
                        desc.polyIdx = polyIdx;
                        matDescriptors.Add(desc);
                    }
                }
            }

            mat.materials = matDescriptors.ToArray();
            if (!ProgramArgs.inst.bQuiet) Console.WriteLine("INFO: MAT parsed successfully.");
            return mat;
        }

        int ParseInt(string str, string line)
        {
            int val;
            if (!Int32.TryParse(str, out val)) throw new ParseException("Expected integer, got \'" + str + "\'", line);
            return val;
        }

        uint ParseUnprefixedHex(string str, string line)
        {
            uint val;
            if (!UInt32.TryParse(str, System.Globalization.NumberStyles.HexNumber,
                System.Globalization.CultureInfo.InvariantCulture, out val))
                throw new ParseException("Expected hex, got \'" + str + "\'", line);
            return val;
        }

        float ParseFloat(string str, string line)
        {
            float val;
            if (!float.TryParse(str, out val)) throw new ParseException("Expected float, got \'" + str + "\'", line);
            return val;
        }

        // Assumes all enum values are the default assigned ones for simplicity
        T ParseEnumInt<T>(string str, string line) where T : IConvertible
        {
            int intVal = ParseInt(str, line);
            Array enumValues = Enum.GetValues(typeof(T));
            if (intVal < 0 || intVal >= enumValues.Length)
                throw new ParseException("Enum integer value " + str + " out of range", line);
            return (T)enumValues.GetValue(intVal);
        }

        RgbColor PartialParseRgb(string[] splitVals, int startIdx, string fileLine)
        {
            if (splitVals.Length < startIdx + 3) throw new ParseException("Unexpected end-of-line while parsing color", fileLine);
            RgbColor color = new RgbColor();
            color.r = ParseInt(splitVals[startIdx], fileLine);
            color.g = ParseInt(splitVals[startIdx + 1], fileLine);
            color.b = ParseInt(splitVals[startIdx + 2], fileLine);
            return color;
        }

        Vector2 PartialParseUv(string[] splitVals, int startIdx, string fileLine)
        {
            if (splitVals.Length < startIdx + 2) throw new ParseException("Unexpected end-of-line while parsing UV", fileLine);
            Vector2 uv = new Vector2();
            uv.x = ParseInt(splitVals[startIdx], fileLine);
            uv.y = ParseInt(splitVals[startIdx + 1], fileLine);
            return uv;
        }

        string ResolvePath(string path, string relativeBase)
        {
            if (System.IO.Path.IsPathRooted(path)) return path;
            else return System.IO.Path.Combine(relativeBase, path);
        }
    }
}
