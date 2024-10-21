using System;
using System.IO;

class Program
{
    static void Main(string[] args)
    {
        int numFiles = 2000; // Number of files to create
        int fileSizeInMb = 1; // Size of each file in MB

        // Set the output directory to the storage/documents path in Termux
        string outputDirectory = "/storage/emulated/0/Documents/output_files";
        Directory.CreateDirectory(outputDirectory); // Create the directory if it doesn't exist

        CreateLargeFiles(outputDirectory, numFiles, fileSizeInMb);
    }

    static void CreateLargeFiles(string outputDirectory, int numFiles, int fileSizeInMb)
    {
        for (int i = 0; i < numFiles; i++)
        {
            string filePath = Path.Combine(outputDirectory, "file_" + i.ToString() + ".txt");
            using (StreamWriter writer = new StreamWriter(filePath))
            {
                // Write a large amount of data (e.g., 'A' repeated many times)
                writer.Write(new string('A', fileSizeInMb * 1024 * 1024)); // 1 MB per file
            }
        }
        Console.WriteLine("Created " + numFiles + " large files in " + outputDirectory);
    }
}
