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
                if (!ParseArgs(args, ref ProgramArgs.inst)) return 1;

                RsdFilesetParser.Parsed mesh;
                try { mesh = new RsdFilesetParser().ParseRsdFileset(ProgramArgs.inst.rsdInput); }
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

                try { new OxmWriter(mesh).WriteToFile(ProgramArgs.inst.oxmOutput, ProgramArgs.inst.scaleFactor); }
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
                catch (System.IO.IOException ex)
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

        private enum ArgState { WantsRsd, WantsOxm, WantsNothing, WantsOptionValue }
        private bool ParseArgs(string[] args, ref ProgramArgs outArgs)
        {
            ArgState prevState = ArgState.WantsRsd;
            ArgState argState = ArgState.WantsRsd;
            string option = "";

            if (args.Length < 2)
            {
                Console.WriteLine("USAGE: oxmesh rsd_input oxm_output [-s scaleFactor] [-q]");
                return false;
            }

            // Default option values
            outArgs.scaleFactor = 64;
            outArgs.bQuiet = false;

            foreach (string arg in args)
            {
                if (arg.StartsWith("-") && arg.Length > 1)
                {
                    if (argState == ArgState.WantsOptionValue)
                    {
                        Console.WriteLine("ARG ERROR: Unexpected option \'" + arg + "\', expected value for previous option \'-" + option + "\'");
                        return false;
                    }

                    option = arg.Substring(1);
                    if (option == "q") outArgs.bQuiet = true;
                    else
                    {
                        prevState = argState;
                        argState = ArgState.WantsOptionValue;
                    }
                    continue;
                }

                switch (argState)
                {
                    case ArgState.WantsRsd:                        
                        outArgs.rsdInput = arg;
                        argState = ArgState.WantsOxm;
                        break;
                    case ArgState.WantsOxm:
                        outArgs.oxmOutput = arg;
                        argState = ArgState.WantsNothing;
                        break;
                    case ArgState.WantsNothing:
                        Console.WriteLine("Unexpected argument \'" + arg + "\'");
                        return false;
                    case ArgState.WantsOptionValue:
                        if (option == "s")
                        {
                            if (!Int32.TryParse(arg, out outArgs.scaleFactor))
                            {
                                Console.WriteLine("Invalid scale factor \'" + arg + "\'");
                                return false;
                            }
                        }

                        argState = prevState;
                        break;
                    default:
                        throw new Exception("lolwat");
                }
            }

            return true;
        }

        static void Main(string[] args)
        {
            int retCode = new OxMesh().Run(args);
            Environment.ExitCode = retCode;
        }
    }
}
