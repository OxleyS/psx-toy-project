using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OxMesh
{
    class OxMesh
    {
        int Run(string[] args)
        {
            try
            {
                RsdFilesetParser.Parsed mesh;

                if (args.Length < 2) { Console.WriteLine("USAGE: oxmesh rsd_input oxm_output"); return 1; }
                string rsdFilename = args[0];

                try { mesh = new RsdFilesetParser().ParseRsdFileset(rsdFilename); }
                catch (RsdFilesetParser.ParseException ex)
                {
                    Console.WriteLine("PARSE ERROR: " + ex.Message);
                    if (ex.fileLine != null)
                    {
                        Console.WriteLine("Offending line:");
                        Console.WriteLine("\t" + ex.fileLine);
                    }
                    return 1;
                }

                if (mesh.ply == null) throw new Exception("RSD needs to specify a PLY file");
                if (mesh.mat == null) throw new Exception("RSD needs to specify a MAT file");

                try { new OxmWriter(mesh).WriteToFile(args[1]); }
                catch (OxmWriter.ConstructException ex)
                {
                    Console.WriteLine("OXM BUILD ERROR: " + ex.Message);
                    return 1;
                }
                catch (OxmWriter.WriteException ex)
                {
                    Console.WriteLine("WRITE ERROR: " + ex.Message);
                    return 1;
                }

                return 0;
            }
            catch (System.IO.FileNotFoundException ex)
            {
                Console.WriteLine("ERROR: Could not find file " + ex.FileName);
                return 1;
            }
            catch (Exception ex)
            {
                Console.WriteLine("FATAL: " + ex.Message);
                Console.WriteLine(ex.StackTrace);
                return 1;
            }
        }

        static void Main(string[] args)
        {
            int retCode = new OxMesh().Run(args);
            Environment.ExitCode = retCode;
        }
    }
}
