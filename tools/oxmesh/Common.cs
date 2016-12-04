using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OxMesh
{
    public struct ProgramArgs
    {
        public static ProgramArgs inst;

        public string rsdInput;
        public string oxmOutput;
        public int scaleFactor;
        public bool bQuiet;
    }

    struct RgbColor
    {
        public RgbColor(int r, int g, int b)
        {
            this.r = r;
            this.g = g;
            this.b = b;
        }

        public int r;
        public int g;
        public int b;
    }

    struct Vector2
    {
        public float x;
        public float y;
    }

    struct Vector3
    {
        public float x;
        public float y;
        public float z;
    }
}
