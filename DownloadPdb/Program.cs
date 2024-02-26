using System.Runtime.InteropServices;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;
using System.Windows;
using Microsoft.VisualBasic;
using System.ComponentModel;
using System.Diagnostics;
using System.Net;
using System.Text.RegularExpressions;
using System.Configuration;

public class Program
{
    private class PeHeaderReader
    {
        #region File Header Structures

        public struct IMAGE_DOS_HEADER
        {
            public UInt16 e_magic;              // Magic number
            public UInt16 e_cblp;               // Bytes on last page of file
            public UInt16 e_cp;                 // Pages in file
            public UInt16 e_crlc;               // Relocations
            public UInt16 e_cparhdr;            // Size of header in paragraphs
            public UInt16 e_minalloc;           // Minimum extra paragraphs needed
            public UInt16 e_maxalloc;           // Maximum extra paragraphs needed
            public UInt16 e_ss;                 // Initial (relative) SS value
            public UInt16 e_sp;                 // Initial SP value
            public UInt16 e_csum;               // Checksum
            public UInt16 e_ip;                 // Initial IP value
            public UInt16 e_cs;                 // Initial (relative) CS value
            public UInt16 e_lfarlc;             // File address of relocation table
            public UInt16 e_ovno;               // Overlay number
            public UInt16 e_res_0;              // Reserved words
            public UInt16 e_res_1;              // Reserved words
            public UInt16 e_res_2;              // Reserved words
            public UInt16 e_res_3;              // Reserved words
            public UInt16 e_oemid;              // OEM identifier (for e_oeminfo)
            public UInt16 e_oeminfo;            // OEM information; e_oemid specific
            public UInt16 e_res2_0;             // Reserved words
            public UInt16 e_res2_1;             // Reserved words
            public UInt16 e_res2_2;             // Reserved words
            public UInt16 e_res2_3;             // Reserved words
            public UInt16 e_res2_4;             // Reserved words
            public UInt16 e_res2_5;             // Reserved words
            public UInt16 e_res2_6;             // Reserved words
            public UInt16 e_res2_7;             // Reserved words
            public UInt16 e_res2_8;             // Reserved words
            public UInt16 e_res2_9;             // Reserved words
            public UInt32 e_lfanew;             // File address of new exe header
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct IMAGE_DATA_DIRECTORY
        {
            public UInt32 VirtualAddress;
            public UInt32 Size;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IMAGE_OPTIONAL_HEADER32
        {
            public UInt16 Magic;
            public Byte MajorLinkerVersion;
            public Byte MinorLinkerVersion;
            public UInt32 SizeOfCode;
            public UInt32 SizeOfInitializedData;
            public UInt32 SizeOfUninitializedData;
            public UInt32 AddressOfEntryPoint;
            public UInt32 BaseOfCode;
            public UInt32 BaseOfData;
            public UInt32 ImageBase;
            public UInt32 SectionAlignment;
            public UInt32 FileAlignment;
            public UInt16 MajorOperatingSystemVersion;
            public UInt16 MinorOperatingSystemVersion;
            public UInt16 MajorImageVersion;
            public UInt16 MinorImageVersion;
            public UInt16 MajorSubsystemVersion;
            public UInt16 MinorSubsystemVersion;
            public UInt32 Win32VersionValue;
            public UInt32 SizeOfImage;
            public UInt32 SizeOfHeaders;
            public UInt32 CheckSum;
            public UInt16 Subsystem;
            public UInt16 DllCharacteristics;
            public UInt32 SizeOfStackReserve;
            public UInt32 SizeOfStackCommit;
            public UInt32 SizeOfHeapReserve;
            public UInt32 SizeOfHeapCommit;
            public UInt32 LoaderFlags;
            public UInt32 NumberOfRvaAndSizes;

            public IMAGE_DATA_DIRECTORY ExportTable;
            public IMAGE_DATA_DIRECTORY ImportTable;
            public IMAGE_DATA_DIRECTORY ResourceTable;
            public IMAGE_DATA_DIRECTORY ExceptionTable;
            public IMAGE_DATA_DIRECTORY CertificateTable;
            public IMAGE_DATA_DIRECTORY BaseRelocationTable;
            public IMAGE_DATA_DIRECTORY Debug;
            public IMAGE_DATA_DIRECTORY Architecture;
            public IMAGE_DATA_DIRECTORY GlobalPtr;
            public IMAGE_DATA_DIRECTORY TLSTable;
            public IMAGE_DATA_DIRECTORY LoadConfigTable;
            public IMAGE_DATA_DIRECTORY BoundImport;
            public IMAGE_DATA_DIRECTORY IAT;
            public IMAGE_DATA_DIRECTORY DelayImportDescriptor;
            public IMAGE_DATA_DIRECTORY CLRRuntimeHeader;
            public IMAGE_DATA_DIRECTORY Reserved;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IMAGE_OPTIONAL_HEADER64
        {
            public UInt16 Magic;
            public Byte MajorLinkerVersion;
            public Byte MinorLinkerVersion;
            public UInt32 SizeOfCode;
            public UInt32 SizeOfInitializedData;
            public UInt32 SizeOfUninitializedData;
            public UInt32 AddressOfEntryPoint;
            public UInt32 BaseOfCode;
            public UInt64 ImageBase;
            public UInt32 SectionAlignment;
            public UInt32 FileAlignment;
            public UInt16 MajorOperatingSystemVersion;
            public UInt16 MinorOperatingSystemVersion;
            public UInt16 MajorImageVersion;
            public UInt16 MinorImageVersion;
            public UInt16 MajorSubsystemVersion;
            public UInt16 MinorSubsystemVersion;
            public UInt32 Win32VersionValue;
            public UInt32 SizeOfImage;
            public UInt32 SizeOfHeaders;
            public UInt32 CheckSum;
            public UInt16 Subsystem;
            public UInt16 DllCharacteristics;
            public UInt64 SizeOfStackReserve;
            public UInt64 SizeOfStackCommit;
            public UInt64 SizeOfHeapReserve;
            public UInt64 SizeOfHeapCommit;
            public UInt32 LoaderFlags;
            public UInt32 NumberOfRvaAndSizes;

            public IMAGE_DATA_DIRECTORY ExportTable;
            public IMAGE_DATA_DIRECTORY ImportTable;
            public IMAGE_DATA_DIRECTORY ResourceTable;
            public IMAGE_DATA_DIRECTORY ExceptionTable;
            public IMAGE_DATA_DIRECTORY CertificateTable;
            public IMAGE_DATA_DIRECTORY BaseRelocationTable;
            public IMAGE_DATA_DIRECTORY Debug;
            public IMAGE_DATA_DIRECTORY Architecture;
            public IMAGE_DATA_DIRECTORY GlobalPtr;
            public IMAGE_DATA_DIRECTORY TLSTable;
            public IMAGE_DATA_DIRECTORY LoadConfigTable;
            public IMAGE_DATA_DIRECTORY BoundImport;
            public IMAGE_DATA_DIRECTORY IAT;
            public IMAGE_DATA_DIRECTORY DelayImportDescriptor;
            public IMAGE_DATA_DIRECTORY CLRRuntimeHeader;
            public IMAGE_DATA_DIRECTORY Reserved;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IMAGE_FILE_HEADER
        {
            public UInt16 Machine;
            public UInt16 NumberOfSections;
            public UInt32 TimeDateStamp;
            public UInt32 PointerToSymbolTable;
            public UInt32 NumberOfSymbols;
            public UInt16 SizeOfOptionalHeader;
            public UInt16 Characteristics;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct IMAGE_SECTION_HEADER
        {
            [FieldOffset(0)]
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public char[] Name;
            [FieldOffset(8)]
            public UInt32 VirtualSize;
            [FieldOffset(12)]
            public UInt32 VirtualAddress;
            [FieldOffset(16)]
            public UInt32 SizeOfRawData;
            [FieldOffset(20)]
            public UInt32 PointerToRawData;
            [FieldOffset(24)]
            public UInt32 PointerToRelocations;
            [FieldOffset(28)]
            public UInt32 PointerToLinenumbers;
            [FieldOffset(32)]
            public UInt16 NumberOfRelocations;
            [FieldOffset(34)]
            public UInt16 NumberOfLinenumbers;
            [FieldOffset(36)]
            public DataSectionFlags Characteristics;

            public string Section
            {
                get { return new string(Name); }
            }
        }

        [Flags]
        public enum DataSectionFlags : uint
        {

            TypeReg = 0x00000000,
            TypeDsect = 0x00000001,
            TypeNoLoad = 0x00000002,
            TypeGroup = 0x00000004,
            TypeNoPadded = 0x00000008,
            TypeCopy = 0x00000010,
            ContentCode = 0x00000020,
            ContentInitializedData = 0x00000040,
            ContentUninitializedData = 0x00000080,
            LinkOther = 0x00000100,
            LinkInfo = 0x00000200,
            TypeOver = 0x00000400,
            LinkRemove = 0x00000800,
            LinkComDat = 0x00001000,
            NoDeferSpecExceptions = 0x00004000,
            RelativeGP = 0x00008000,
            MemPurgeable = 0x00020000,
            Memory16Bit = 0x00020000,
            MemoryLocked = 0x00040000,
            MemoryPreload = 0x00080000,
            Align1Bytes = 0x00100000,
            Align2Bytes = 0x00200000,
            Align4Bytes = 0x00300000,
            Align8Bytes = 0x00400000,
            Align16Bytes = 0x00500000,
            Align32Bytes = 0x00600000,
            Align64Bytes = 0x00700000,
            Align128Bytes = 0x00800000,
            Align256Bytes = 0x00900000,
            Align512Bytes = 0x00A00000,
            Align1024Bytes = 0x00B00000,
            Align2048Bytes = 0x00C00000,
            Align4096Bytes = 0x00D00000,
            Align8192Bytes = 0x00E00000,
            LinkExtendedRelocationOverflow = 0x01000000,
            MemoryDiscardable = 0x02000000,
            MemoryNotCached = 0x04000000,
            MemoryNotPaged = 0x08000000,
            MemoryShared = 0x10000000,
            MemoryExecute = 0x20000000,
            MemoryRead = 0x40000000,
            MemoryWrite = 0x80000000
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IMAGE_DEBUG_DIRECTORY
        {
            public UInt32 Characteristics;
            public UInt32 TimeDateStamp;
            public UInt16 MajorVersion;
            public UInt16 MinorVersion;
            public UInt32 Type;
            public UInt32 SizeOfData;
            public UInt32 AddressOfRawData;
            public UInt32 PointerToRawData;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IMAGE_DEBUG_DIRECTORY_RAW
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public char[] format;
            public Guid guid;
            public uint age;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 255)]
            public char[] name;
        }

        #endregion File Header Structures

        #region Private Fields

        private IMAGE_DOS_HEADER dosHeader;
        private IMAGE_FILE_HEADER fileHeader;
        private IMAGE_OPTIONAL_HEADER32 optionalHeader32;
        private IMAGE_OPTIONAL_HEADER64 optionalHeader64;
        private IMAGE_SECTION_HEADER[] imageSectionHeaders;

        private IMAGE_DEBUG_DIRECTORY imageDebugDirectory;
        private IMAGE_DEBUG_DIRECTORY_RAW DebugInfo;

        private string _pdbName = "";
        private string _pdbage = "";
        private Guid _debugGUID;

        #endregion Private Fields

        #region Public Methods

        public PeHeaderReader(string filePath)
        {
            using (FileStream stream = new FileStream(filePath, System.IO.FileMode.Open, System.IO.FileAccess.Read))
            {
                BinaryReader reader = new BinaryReader(stream);
                dosHeader = FromBinaryReader<IMAGE_DOS_HEADER>(reader);

                // Add 4 bytes to the offset
                stream.Seek(dosHeader.e_lfanew, SeekOrigin.Begin);

                UInt32 ntHeadersSignature = reader.ReadUInt32();
                fileHeader = FromBinaryReader<IMAGE_FILE_HEADER>(reader);
                if (this.Is32BitHeader)
                {
                    optionalHeader32 = FromBinaryReader<IMAGE_OPTIONAL_HEADER32>(reader);
                }
                else
                {
                    optionalHeader64 = FromBinaryReader<IMAGE_OPTIONAL_HEADER64>(reader);
                }

                uint offDebug = 0;
                uint cbDebug = 0;
                long cbFromHeader = 0;
                int loopexit = 0;

                if (this.Is32BitHeader)
                    cbDebug = optionalHeader32.Debug.Size;
                else
                    cbDebug = optionalHeader64.Debug.Size;

                imageSectionHeaders = new IMAGE_SECTION_HEADER[fileHeader.NumberOfSections];
                for (int headerNo = 0; headerNo < imageSectionHeaders.Length; ++headerNo)
                {
                    imageSectionHeaders[headerNo] = FromBinaryReader<IMAGE_SECTION_HEADER>(reader);

                    if ((imageSectionHeaders[headerNo].PointerToRawData != 0) &&
                            (imageSectionHeaders[headerNo].SizeOfRawData != 0) &&
                                (cbFromHeader < (long)
                                    (imageSectionHeaders[headerNo].PointerToRawData + imageSectionHeaders[headerNo].SizeOfRawData)))
                    {
                        cbFromHeader = (long)
                            (imageSectionHeaders[headerNo].PointerToRawData + imageSectionHeaders[headerNo].SizeOfRawData);
                    }

                    if (cbDebug != 0)
                    {
                        if (this.Is32BitHeader)
                        {
                            if (imageSectionHeaders[headerNo].VirtualAddress <= optionalHeader32.Debug.VirtualAddress &&
                                    ((imageSectionHeaders[headerNo].VirtualAddress + imageSectionHeaders[headerNo].SizeOfRawData) > optionalHeader32.Debug.VirtualAddress))
                            {
                                offDebug = optionalHeader32.Debug.VirtualAddress - imageSectionHeaders[headerNo].VirtualAddress + imageSectionHeaders[headerNo].PointerToRawData;
                            }
                        }
                        else
                        {
                            if (imageSectionHeaders[headerNo].VirtualAddress <= optionalHeader64.Debug.VirtualAddress &&
                                ((imageSectionHeaders[headerNo].VirtualAddress + imageSectionHeaders[headerNo].SizeOfRawData) > optionalHeader64.Debug.VirtualAddress))
                            {
                                offDebug = optionalHeader64.Debug.VirtualAddress - imageSectionHeaders[headerNo].VirtualAddress + imageSectionHeaders[headerNo].PointerToRawData;
                            }
                        }
                    }
                }

                stream.Seek(offDebug, SeekOrigin.Begin);

                while (cbDebug >= Marshal.SizeOf(typeof(IMAGE_DEBUG_DIRECTORY)))
                {
                    if (loopexit == 0)
                    {
                        imageDebugDirectory = FromBinaryReader<IMAGE_DEBUG_DIRECTORY>(reader);
                        long seekPosition = stream.Position;

                        if (imageDebugDirectory.Type == 0x2)
                        {
                            stream.Seek(imageDebugDirectory.PointerToRawData, SeekOrigin.Begin);
                            DebugInfo = FromBinaryReader<IMAGE_DEBUG_DIRECTORY_RAW>(reader);
                            loopexit = 1;

                            //Downloading logic for .NET native images
                            if (new string(DebugInfo.name).Contains(".ni."))
                            {
                                stream.Seek(seekPosition, SeekOrigin.Begin);
                                loopexit = 0;
                            }
                        }

                        if ((imageDebugDirectory.PointerToRawData != 0) &&
                                (imageDebugDirectory.SizeOfData != 0) &&
                                (cbFromHeader < (long)
                                    (imageDebugDirectory.PointerToRawData + imageDebugDirectory.SizeOfData)))
                        {
                            cbFromHeader = (long)
                                (imageDebugDirectory.PointerToRawData + imageDebugDirectory.SizeOfData);
                        }
                    }

                    cbDebug -= (uint)Marshal.SizeOf(typeof(IMAGE_DEBUG_DIRECTORY));
                }

                if (loopexit != 0)
                {
                    _pdbName = new string(DebugInfo.name);
                    _pdbName = _pdbName.Remove(_pdbName.IndexOf("\0"));

                    _pdbage = DebugInfo.age.ToString("X");
                    _debugGUID = DebugInfo.guid;
                }

            }
        }

        public static T FromBinaryReader<T>(BinaryReader reader)
        {
            byte[] bytes = reader.ReadBytes(Marshal.SizeOf(typeof(T)));

            GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            T theStructure = (T)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(T));
            handle.Free();

            return theStructure;
        }

        #endregion Public Methods

        #region Properties

        public string pdbName { get { return _pdbName; } }
        public string pdbage { get { return _pdbage; } }
        public Guid debugGUID { get { return _debugGUID; } }

        public bool Is32BitHeader
        {
            get
            {
                //UInt16 IMAGE_FILE_32BIT_MACHINE = 0x0100;
                //return (IMAGE_FILE_32BIT_MACHINE & FileHeader.Characteristics) == IMAGE_FILE_32BIT_MACHINE;
                return (FileHeader.Machine == 332) ? true : false; //14C = X86
            }
        }

        public IMAGE_FILE_HEADER FileHeader
        {
            get
            {
                return fileHeader;
            }
        }

        public IMAGE_OPTIONAL_HEADER32 OptionalHeader32
        {
            get
            {
                return optionalHeader32;
            }
        }

        public IMAGE_OPTIONAL_HEADER64 OptionalHeader64
        {
            get
            {
                return optionalHeader64;
            }
        }

        public IMAGE_SECTION_HEADER[] ImageSectionHeaders
        {
            get
            {
                return imageSectionHeaders;
            }
        }

        #endregion Properties

        #region FileDownloader
        class ResourceDownloader : System.Object, IDisposable
        {

            #region Nested Types
            public struct FileInfo
            {
                public string Path;
                public string Name;
                public string PdbGuid;
                public bool IsCompressed;

                public FileInfo(String path)
                {
                    this.Path = path;
                    this.Name = this.Path.Split("/"[0])[this.Path.Split("/"[0]).Length - 1];
                    this.PdbGuid = this.Path.Split("/"[0])[this.Path.Split("/"[0]).Length - 2];
                    this.IsCompressed = false;
                }

                public void SetPath(string path)
                {
                    this.Path = path;
                }

                public void SetName()
                {
                    this.Name = this.Path.Split("/"[0])[this.Path.Split("/"[0]).Length - 1];
                }
                public void SetNameUsingPath(string path)
                {
                    this.Name = path;
                }

            }


            private enum Event
            {
                CalculationFileSizesStarted,

                FileSizesCalculationComplete,
                DeletingFilesAfterCancel,

                FileDownloadAttempting,
                FileDownloadStarted,
                FileDownloadStopped,
                FileDownloadSucceeded,

                ProgressChanged
            };

            private enum InvokeType
            {
                EventRaiser,
                FileDownloadFailedRaiser,
                CalculatingFileNrRaiser
            };
            #endregion

            #region Events
            public event EventHandler Started;
            public event EventHandler Paused;
            public event EventHandler Resumed;
            public event EventHandler CancelRequested;
            public event EventHandler DeletingFilesAfterCancel;
            public event EventHandler Canceled;
            public event EventHandler Completed;
            public event EventHandler Stopped;

            public event EventHandler IsBusyChanged;
            public event EventHandler IsPausedChanged;
            public event EventHandler StateChanged;

            public event EventHandler CalculationFileSizesStarted;
            public event CalculatingFileSizeEventHandler CalculatingFileSize;
            public event EventHandler FileSizesCalculationComplete;

            public event EventHandler FileDownloadAttempting;
            public event EventHandler FileDownloadStarted;
            public event EventHandler FileDownloadStopped;
            public event EventHandler FileDownloadSucceeded;
            public event FailEventHandler FileDownloadFailed;

            public event EventHandler ProgressChanged;
            #endregion

            #region Fields
            private const Int32 default_decimals = 2;

            // Delegates
            public delegate void FailEventHandler(object sender, Exception ex);
            public delegate void CalculatingFileSizeEventHandler(object sender, Int32 fileNr);

            // The download worker
            private BackgroundWorker bgwDownloader = new BackgroundWorker();

            // Preferences
            private Boolean m_supportsProgress, m_deleteCompletedFiles;
            private Int32 m_packageSize, m_stopWatchCycles;
            public string DownloadLocation;

            // State
            private Boolean m_disposed = false;
            private Boolean m_busy, m_paused, m_canceled;
            private Int64 m_currentFileProgress, m_totalProgress, m_currentFileSize;
            private Int32 m_currentSpeed, m_fileNr;

            // Data
            private String m_localDirectory;
            private List<FileInfo> m_files = new List<FileInfo>();
            private Int64 m_totalSize;

            #endregion

            #region Constructors
            public ResourceDownloader()
            {
                this.initizalize(false);
            }

            public ResourceDownloader(Boolean supportsProgress)
            {
                this.initizalize(supportsProgress);
            }

            private void initizalize(Boolean supportsProgress)
            {
                // Set the bgw properties
                bgwDownloader.WorkerReportsProgress = true;
                bgwDownloader.WorkerSupportsCancellation = true;
                bgwDownloader.DoWork += new DoWorkEventHandler(bgwDownloader_DoWork);
                bgwDownloader.RunWorkerCompleted += new RunWorkerCompletedEventHandler(bgwDownloader_RunWorkerCompleted);
                bgwDownloader.ProgressChanged += new ProgressChangedEventHandler(bgwDownloader_ProgressChanged);

                // Set the default class preferences
                this.SupportsProgress = supportsProgress;
                this.PackageSize = 4096;
                this.StopWatchCyclesAmount = 5;
                this.DeleteCompletedFilesAfterCancel = true;
                this.DownloadLocation = !string.IsNullOrEmpty(Constants.DownloadFolder) ? Constants.DownloadFolder : "C:\\symcache";
            }
            #endregion

            #region Public methods
            public void Start() { this.IsBusy = true; }

            public void Pause() { this.IsPaused = true; }

            public void Resume() { this.IsPaused = false; }

            public void Stop() { this.IsBusy = false; }
            public void Stop(Boolean deleteCompletedFiles)
            {
                this.DeleteCompletedFilesAfterCancel = deleteCompletedFiles;
                this.Stop();
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            public void SetDownloadLocation(string path)
            {
                DownloadLocation = path;
            }

            #region Size formatting functions
            public static string FormatSizeBinary(Int64 size)
            {
                return ResourceDownloader.FormatSizeBinary(size, default_decimals);
            }

            public static string FormatSizeBinary(Int64 size, Int32 decimals)
            {
                String[] sizes = { "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
                Double formattedSize = size;
                Int32 sizeIndex = 0;
                while (formattedSize >= 1024 && sizeIndex < sizes.Length)
                {
                    formattedSize /= 1024;
                    sizeIndex += 1;
                }
                return Math.Round(formattedSize, decimals) + sizes[sizeIndex];
            }

            public static string FormatSizeDecimal(Int64 size)
            {
                return ResourceDownloader.FormatSizeDecimal(size, default_decimals);
            }

            public static string FormatSizeDecimal(Int64 size, Int32 decimals)
            {
                String[] sizes = { "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
                Double formattedSize = size;
                Int32 sizeIndex = 0;
                while (formattedSize >= 1000 && sizeIndex < sizes.Length)
                {
                    formattedSize /= 1000;
                    sizeIndex += 1;
                }
                return Math.Round(formattedSize, decimals) + sizes[sizeIndex];
            }
            #endregion

            #endregion

            #region Protected methods
            protected virtual void Dispose(Boolean disposing)
            {
                if (!m_disposed)
                {
                    if (disposing)
                    {
                        bgwDownloader.Dispose();
                    }
                    this.Files = null;
                }
            }
            #endregion

            #region Private methods
            private void bgwDownloader_DoWork(object sender, DoWorkEventArgs e)
            {
                Int32 fileNr = 0;

                if (this.SupportsProgress) { calculateFilesSize(); }

                if (!Directory.Exists(this.LocalDirectory)) { Directory.CreateDirectory(this.LocalDirectory); }

                while (fileNr < this.Files.Count && !bgwDownloader.CancellationPending)
                {
                    m_fileNr = fileNr;
                    downloadFile(fileNr);

                    if (bgwDownloader.CancellationPending)
                    {
                        fireEventFromBgw(Event.DeletingFilesAfterCancel);
                        cleanUpFiles(this.DeleteCompletedFilesAfterCancel ? 0 : m_fileNr, this.DeleteCompletedFilesAfterCancel ? m_fileNr + 1 : 1);
                    }
                    else
                    {
                        fileNr += 1;
                    }
                }
            }

            private void calculateFilesSize()
            {
                fireEventFromBgw(Event.CalculationFileSizesStarted);
                bool headVerb = true;
                m_totalSize = 0;
                string message;
                for (Int32 fileNr = 0; fileNr < this.Files.Count; fileNr++)
                {
                    bgwDownloader.ReportProgress((Int32)InvokeType.CalculatingFileNrRaiser, fileNr + 1);
                    try
                    {
                        //Probe 1
                        HttpWebRequest webReq = (HttpWebRequest)WebRequest.Create(this.Files[fileNr].Path);
                        webReq.UserAgent = Constants.SymbolServer;
                        webReq.Method = "HEAD";
                        HttpWebResponse webResp = (HttpWebResponse)webReq.GetResponseNoException();

                        //Probe 2
                        if (webResp.StatusCode == HttpStatusCode.NotFound)
                        {
                            webResp = Retry(fileNr, headVerb);
                        }

                        if (webResp.StatusCode == HttpStatusCode.OK)
                        {
                            m_totalSize += webResp.ContentLength;
                        }

                        //Probe 3
                        if (webResp.StatusCode == HttpStatusCode.NotFound)
                        {
                            webResp = RetryFilePointer(fileNr);

                            if (webResp.StatusCode == HttpStatusCode.OK)
                            {
                                string ignore = null;
                                m_totalSize += ProcessFileSize(webResp, out ignore);
                            }
                        }


                        webResp.Close();
                    }
                    catch (Exception ex)
                    {
                        message = ex.Message.ToString();
                    }
                }
                fireEventFromBgw(Event.FileSizesCalculationComplete);
            }

            private HttpWebResponse Retry(int fileNr, bool headVerb)
            {
                string path = this.Files[fileNr].Path;
                path = ProbeWithUnderscore(path);
                var webReq = (HttpWebRequest)System.Net.WebRequest.Create(path);
                webReq.UserAgent = Constants.SymbolServer;
                if (headVerb)
                    webReq.Method = "HEAD";
                return (HttpWebResponse)webReq.GetResponseNoException();
            }

            private HttpWebResponse RetryFilePointer(int fileNr)
            {
                string path = this.Files[fileNr].Path;
                path = ProbeWithFilePointer(path);
                var webReq = (HttpWebRequest)System.Net.WebRequest.Create(path);
                webReq.UserAgent = Constants.SymbolServer;
                return (HttpWebResponse)webReq.GetResponseNoException();
            }

            private long ProcessFileSize(HttpWebResponse webResp, out string filePath)
            {
                long length = 0;
                filePath = null;
                Stream receiveStream = webResp.GetResponseStream();
                Encoding encode = System.Text.Encoding.GetEncoding("utf-8");
                StreamReader readStream = new StreamReader(receiveStream, encode);
                Char[] read = new Char[webResp.ContentLength];
                readStream.Read(read, 0, (int)webResp.ContentLength);

                string file = new string(read, 0, (int)webResp.ContentLength);

                if (file.Contains("PATH"))
                {
                    file = file.Substring(5, file.Length - 5); //Removing PATH: from the output

                    try
                    {
                        System.IO.FileInfo fInfo = new System.IO.FileInfo(file);
                        if (fInfo.Exists)
                        {
                            length = fInfo.Length;
                            filePath = file;
                        }
                    }
                    catch (Exception ex)
                    {
                        WriteToLog(file, ex);
                    }
                }
                else
                {
                    int position = webResp.ResponseUri.PathAndQuery.IndexOf(".pdb");
                    string fileName = webResp.ResponseUri.PathAndQuery.Substring(1, position + 3);
                    if (!FailedFiles.ContainsKey(fileName))
                        FailedFiles.Add(fileName, " - No matching PDBs found - " + file);
                }

                return length;
            }

            private void DownloadFile(string srcFile, string filePath)
            {
                File.Copy(srcFile, filePath, true);
            }

            private static string ProbeWithUnderscore(string path)
            {
                path = path.Remove(path.Length - 1);
                path = path.Insert(path.Length, "_");
                return path;
            }

            private static string ProbeWithFilePointer(string path)
            {
                int position = path.LastIndexOf('/');
                path = path.Remove(position, (path.Length - position));
                path = path.Insert(path.Length, "/file.ptr");
                return path;
            }

            private void fireEventFromBgw(Event eventName)
            {
                bgwDownloader.ReportProgress((int)InvokeType.EventRaiser, eventName);
            }

            private void downloadFile(Int32 fileNr)
            {
                bool headVerb = false;
                m_currentFileSize = 0;
                bool fileptr = false;
                fireEventFromBgw(Event.FileDownloadAttempting);

                FileInfo file = this.Files[fileNr];

                Int64 size = 0;

                Byte[] readBytes = new Byte[this.PackageSize];
                Int32 currentPackageSize;
                System.Diagnostics.Stopwatch speedTimer = new System.Diagnostics.Stopwatch();
                Int32 readings = 0;
                Exception exc = null;

                FileStream writer;
                string dirPath = DownloadLocation + "\\" + file.Name + "\\" + file.PdbGuid;
                string downloadUrl = this.Files[fileNr].Path;

                HttpWebRequest webReq;
                HttpWebResponse webResp = null;

                try
                {
                    webReq = (HttpWebRequest)System.Net.WebRequest.Create(downloadUrl);
                    webReq.UserAgent = Constants.SymbolServer;
                    webResp = (HttpWebResponse)webReq.GetResponseNoException();
                    if (webResp.StatusCode == HttpStatusCode.NotFound)
                    {
                        webResp = Retry(fileNr, headVerb);

                        if (webResp.StatusCode == HttpStatusCode.OK)
                        {
                            file.IsCompressed = true;
                            size = webResp.ContentLength;
                        }

                        if (webResp.StatusCode == HttpStatusCode.NotFound)
                        {
                            webResp = RetryFilePointer(fileNr);
                            fileptr = true;
                        }

                        if (webResp.StatusCode != HttpStatusCode.OK)
                        {
                            if (!FailedFiles.ContainsKey(file.Name))
                                FailedFiles.Add(file.Name, " - " + webResp.StatusCode + "  " + webResp.StatusDescription);
                        }
                    }
                    else if (webResp.StatusCode == HttpStatusCode.OK)
                        size = webResp.ContentLength;

                }
                catch (Exception ex)
                {
                    exc = ex;
                    WriteToLog(file.Name, exc);
                }
                if (webResp.StatusCode == HttpStatusCode.OK)
                {
                    Directory.CreateDirectory(dirPath);

                    if (fileptr)
                    {
                        string filePath = dirPath + "\\" +
                            file.Name;
                        string srcFile = null;
                        FileStream reader;
                        size = ProcessFileSize(webResp, out srcFile);
                        m_currentFileSize = size;

                        if (srcFile != null)
                        {
                            reader = new FileStream(srcFile, FileMode.Open, FileAccess.Read);
                            writer = new FileStream(filePath,
                                System.IO.FileMode.Create);

                            //   DownloadFile(srcFile, filePath);
                            fireEventFromBgw(Event.FileDownloadStarted);
                            m_currentFileProgress = 0;
                            while (m_currentFileProgress < size && !bgwDownloader.CancellationPending)
                            {
                                while (this.IsPaused) { System.Threading.Thread.Sleep(100); }

                                speedTimer.Start();

                                currentPackageSize = reader.Read(readBytes, 0, this.PackageSize);

                                m_currentFileProgress += currentPackageSize;
                                m_totalProgress += currentPackageSize;
                                fireEventFromBgw(Event.ProgressChanged);

                                writer.Write(readBytes, 0, currentPackageSize);
                                readings += 1;

                                if (readings >= this.StopWatchCyclesAmount)
                                {
                                    m_currentSpeed = (Int32)(this.PackageSize * StopWatchCyclesAmount * 1000 / (speedTimer.ElapsedMilliseconds + 1));
                                    speedTimer.Reset();
                                    readings = 0;
                                }
                            }
                            reader.Close();
                            writer.Close();
                            speedTimer.Stop();
                            //end
                        }
                    }
                    else
                    {
                        m_currentFileSize = size;
                        //string name;
                        if (file.IsCompressed)
                        {
                            file.Name = ProbeWithUnderscore(file.Name);
                        }
                        string filePath = dirPath + "\\" +
                            file.Name;
                        writer = new FileStream(filePath,
                            System.IO.FileMode.Create);

                        if (exc != null)
                        {
                            bgwDownloader.ReportProgress((Int32)InvokeType.FileDownloadFailedRaiser, exc);
                        }
                        else
                        {
                            m_currentFileProgress = 0;
                            while (m_currentFileProgress < size && !bgwDownloader.CancellationPending)
                            {
                                while (this.IsPaused) { System.Threading.Thread.Sleep(100); }

                                speedTimer.Start();

                                currentPackageSize = webResp.GetResponseStream().Read(readBytes, 0, this.PackageSize);

                                m_currentFileProgress += currentPackageSize;
                                m_totalProgress += currentPackageSize;
                                fireEventFromBgw(Event.ProgressChanged);

                                writer.Write(readBytes, 0, currentPackageSize);
                                readings += 1;

                                if (readings >= this.StopWatchCyclesAmount)
                                {
                                    m_currentSpeed = (Int32)(this.PackageSize * StopWatchCyclesAmount * 1000 / (speedTimer.ElapsedMilliseconds + 1));
                                    speedTimer.Reset();
                                    readings = 0;
                                }
                            }

                            speedTimer.Stop();
                            writer.Close();

                            webResp.Close();
                            if (file.IsCompressed)
                            {
                                HandleCompression(filePath);
                            }

                        }
                        if (!bgwDownloader.CancellationPending) { fireEventFromBgw(Event.FileDownloadSucceeded); }
                    }
                }
                fireEventFromBgw(Event.FileDownloadStopped);
            }

            public static void WriteToLog(string fileName, Exception exc)
            {
                using (FileStream fs = new FileStream("Log.txt", FileMode.Append))
                using (StreamWriter sr = new StreamWriter(fs))
                {
                    sr.WriteLine(DateTime.Now.ToString() + "   " + fileName + " - " + exc.Message);
                }
            }

            public static void WriteToLog(string fileName, string text)
            {
                using (FileStream fs = new FileStream("Log.txt", FileMode.Append))
                using (StreamWriter sr = new StreamWriter(fs))
                {
                    sr.WriteLine(DateTime.Now.ToString() + "   " + fileName + " - " + text);
                }
            }


            private void HandleCompression(string filePath)
            {
                string uncompressedFilePath = filePath.Remove(filePath.Length - 1);
                uncompressedFilePath = uncompressedFilePath.Insert(uncompressedFilePath.Length, "b");
                string args = string.Format("expand {0} {1}", "\"" + filePath + "\"", "\"" + uncompressedFilePath + "\"");

                Match m = Regex.Match(args, "^\\s*\"(.*?)\"\\s*(.*)");
                if (!m.Success)
                    m = Regex.Match(args, @"\s*(\S*)\s*(.*)");    // thing before first space is command

                Process process = new Process();
                ProcessStartInfo startInfo = new ProcessStartInfo(m.Groups[1].Value, m.Groups[2].Value);

                startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;

                startInfo.UseShellExecute = false;
                startInfo.Verb = "runas";
                startInfo.CreateNoWindow = true;
                process.StartInfo = startInfo;

                try
                {
                    var started = process.Start();
                    if (started)
                    {
                        process.WaitForExit(600000);
                        File.Delete(filePath);
                    }
                }
                catch (Exception ex)
                {
                    WriteToLog(filePath, ex);
                }
            }

            private void cleanUpFiles(Int32 start, Int32 length)
            {
                Int32 last = length < 0 ? this.Files.Count - 1 : start + length - 1;

                for (Int32 fileNr = start; fileNr <= last; fileNr++)
                {
                    String fullPath = this.LocalDirectory + "\\" + this.Files[fileNr].Name;
                    if (System.IO.File.Exists(fullPath)) { System.IO.File.Delete(fullPath); }
                }
            }

            private void bgwDownloader_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
            {
                m_paused = false;
                m_busy = false;

                if (this.HasBeenCanceled)
                {
                    if (Canceled != null) { this.Canceled(this, new EventArgs()); }
                }
                else
                {
                    if (Completed != null) { this.Completed(this, new EventArgs()); }
                }

                if (Stopped != null) { this.Stopped(this, new EventArgs()); }
                if (IsBusyChanged != null) { this.IsBusyChanged(this, new EventArgs()); }
                if (StateChanged != null) { this.StateChanged(this, new EventArgs()); }
            }

            private void bgwDownloader_ProgressChanged(object sender, ProgressChangedEventArgs e)
            {
                switch ((InvokeType)e.ProgressPercentage)
                {
                    case InvokeType.EventRaiser:
                        switch ((Event)e.UserState)
                        {
                            case Event.CalculationFileSizesStarted:
                                if (CalculationFileSizesStarted != null) { this.CalculationFileSizesStarted(this, new EventArgs()); }
                                break;
                            case Event.FileSizesCalculationComplete:
                                if (FileSizesCalculationComplete != null) { this.FileSizesCalculationComplete(this, new EventArgs()); }
                                break;
                            case Event.DeletingFilesAfterCancel:
                                if (DeletingFilesAfterCancel != null) { this.DeletingFilesAfterCancel(this, new EventArgs()); }
                                break;

                            case Event.FileDownloadAttempting:
                                if (FileDownloadAttempting != null) { this.FileDownloadAttempting(this, new EventArgs()); }
                                break;
                            case Event.FileDownloadStarted:
                                if (FileDownloadStarted != null) { this.FileDownloadStarted(this, new EventArgs()); }
                                break;
                            case Event.FileDownloadStopped:
                                if (FileDownloadStopped != null) { this.FileDownloadStopped(this, new EventArgs()); }
                                break;
                            case Event.FileDownloadSucceeded:
                                if (FileDownloadSucceeded != null) { this.FileDownloadSucceeded(this, new EventArgs()); }
                                break;
                            case Event.ProgressChanged:
                                if (ProgressChanged != null) { this.ProgressChanged(this, new EventArgs()); }
                                break;
                        }
                        break;
                    case InvokeType.FileDownloadFailedRaiser:
                        if (FileDownloadFailed != null) { this.FileDownloadFailed(this, (Exception)e.UserState); }
                        break;
                    case InvokeType.CalculatingFileNrRaiser:
                        if (CalculatingFileSize != null) { this.CalculatingFileSize(this, (Int32)e.UserState); }
                        break;
                }
            }
            #endregion

            #region Properties
            public List<FileInfo> Files
            {
                get { return m_files; }
                set
                {
                    if (this.IsBusy)
                    {
                        throw new InvalidOperationException("You can not change the file list during the download");
                    }
                    else
                    {
                        if (this.Files != null) m_files = value;
                    }
                }
            }

            public Dictionary<string, string> FailedFiles = new Dictionary<string, string>();

            public String LocalDirectory
            {
                get { return m_localDirectory; }
                set
                {
                    if (this.LocalDirectory != value) { m_localDirectory = value; }
                }
            }

            public Boolean SupportsProgress
            {
                get { return m_supportsProgress; }
                set
                {
                    if (this.IsBusy)
                    {
                        throw new InvalidOperationException("You can not change the SupportsProgress property during the download");
                    }
                    else
                    {
                        m_supportsProgress = value;
                    }
                }
            }

            public Boolean DeleteCompletedFilesAfterCancel
            {
                get { return m_deleteCompletedFiles; }
                set { m_deleteCompletedFiles = value; }
            }

            public Int32 PackageSize
            {
                get { return m_packageSize; }
                set
                {
                    if (value > 0)
                    {
                        m_packageSize = value;
                    }
                    else
                    {
                        throw new InvalidOperationException("The PackageSize needs to be greather then 0");
                    }
                }
            }

            public Int32 StopWatchCyclesAmount
            {
                get { return m_stopWatchCycles; }
                set
                {
                    if (value > 0)
                    {
                        m_stopWatchCycles = value;
                    }
                    else
                    {
                        throw new InvalidOperationException("The StopWatchCyclesAmount needs to be greather then 0");
                    }
                }
            }

            public Boolean IsBusy
            {
                get { return m_busy; }
                set
                {
                    if (this.IsBusy != value)
                    {
                        m_busy = value;
                        m_canceled = !value;
                        if (this.IsBusy)
                        {
                            m_totalProgress = 0;
                            bgwDownloader.RunWorkerAsync();

                            if (Started != null) { this.Started(this, new EventArgs()); }
                            if (IsBusyChanged != null) { this.IsBusyChanged(this, new EventArgs()); }
                            if (StateChanged != null) { this.StateChanged(this, new EventArgs()); }
                        }
                        else
                        {
                            m_paused = false;
                            bgwDownloader.CancelAsync();
                            if (CancelRequested != null) { this.CancelRequested(this, new EventArgs()); }
                            if (StateChanged != null) { this.StateChanged(this, new EventArgs()); }
                        }
                    }
                }
            }

            public Boolean IsPaused
            {
                get { return m_paused; }
                set
                {
                    if (this.IsBusy)
                    {
                        if (this.IsPaused != value)
                        {
                            m_paused = value;
                            if (this.IsPaused)
                            {
                                if (Paused != null) { this.Paused(this, new EventArgs()); }
                            }
                            else
                            {
                                if (Resumed != null) { this.Resumed(this, new EventArgs()); }
                            }
                            if (IsPausedChanged != null) { this.IsPausedChanged(this, new EventArgs()); }
                            if (StateChanged != null) { this.StateChanged(this, new EventArgs()); }
                        }
                    }
                    else
                    {
                        throw new InvalidOperationException("You can not change the IsPaused property when the FileDownloader is not busy");
                    }
                }
            }

            public Boolean CanStart
            {
                get { return !this.IsBusy; }
            }

            public Boolean CanPause
            {
                get { return this.IsBusy && !this.IsPaused && !bgwDownloader.CancellationPending; }
            }

            public Boolean CanResume
            {
                get { return this.IsBusy && this.IsPaused && !bgwDownloader.CancellationPending; }
            }

            public Boolean CanStop
            {
                get { return this.IsBusy && !bgwDownloader.CancellationPending; }
            }

            public Int64 TotalSize
            {
                get
                {
                    if (this.SupportsProgress)
                    {
                        return m_totalSize;
                    }
                    else
                    {
                        throw new InvalidOperationException("This FileDownloader that it doesn't support progress. Modify SupportsProgress to state that it does support progress to get the total size.");
                    }
                }
            }

            public Int64 TotalProgress
            {
                get { return m_totalProgress; }
            }

            public Int64 CurrentFileProgress
            {
                get { return m_currentFileProgress; }
            }

            public Double TotalPercentage()
            {
                return this.TotalPercentage(default_decimals);
            }

            public Double TotalPercentage(Int32 decimals)
            {
                if (this.SupportsProgress)
                {
                    return Math.Round((Double)this.TotalProgress / this.TotalSize * 100, decimals);
                }
                else
                {
                    throw new InvalidOperationException("This FileDownloader that it doesn't support progress. Modify SupportsProgress to state that it does support progress.");
                }
            }

            public Double CurrentFilePercentage()
            {
                return this.CurrentFilePercentage(default_decimals);
            }

            public Double CurrentFilePercentage(Int32 decimals)
            {
                return Math.Round((Double)this.CurrentFileProgress / this.CurrentFileSize * 100, decimals);
            }

            public Int32 DownloadSpeed
            {
                get { return m_currentSpeed; }
            }

            public FileInfo CurrentFile
            {
                get { return this.Files[m_fileNr]; }
            }

            public Int64 CurrentFileSize
            {
                get { return m_currentFileSize; }
            }

            public Boolean HasBeenCanceled
            {
                get { return m_canceled; }
            }
            #endregion

        }
        #endregion


        public static class Constants
        {

            #region Settings
            public static string SymbolServer;
            public static string DownloadFolder;
            public static bool EnableBulkDownload;
            #endregion

            #region Ctor
            static Constants()
            {
                SymbolServer = @"Microsoft-Symbol-Server/10.0.10522.521";
                DownloadFolder = @"c:\symbols";
                EnableBulkDownload = false;
            }

            #endregion
        }


        public class HttpWebResponseExt
        {
        }

        public static class ConfigurationReader
        {
            #region Settings
            public static string SymbolServerUrl;
            #endregion

            #region Ctor
            static ConfigurationReader()
            {
                SymbolServerUrl = ConfigurationManager.AppSettings["SymbolServer"];
            }
            #endregion
        }

        public static string BuildUrl(string filename)
        {
            string downloadURL = string.Empty;
            string SymbolServerUrl;

            if (File.Exists(filename))
            {
                PeHeaderReader reader = new PeHeaderReader(filename);
                //UserAgent:  Microsoft-Symbol-Server/10.0.10036.206
                //Host:  msdl.microsoft.com
                //URI: /download/symbols/iiscore.pdb/6E3058DA562C4EB187071DC08CF7B59E1/iiscore.pdb
                string pdbName;

                if (string.IsNullOrEmpty(reader.pdbName))
                {
                    downloadURL = string.Empty;
                }
                else
                {
                    if (reader.pdbName.Contains("\\"))
                    {
                        pdbName = (reader.pdbName.Split(new char[] { '\\' }))[reader.pdbName.Split(new char[] { '\\' }).Length - 1];
                    }
                    else
                        pdbName = reader.pdbName;

                    SymbolServerUrl = ConfigurationReader.SymbolServerUrl;

                    if (string.IsNullOrEmpty(SymbolServerUrl))
                        downloadURL = "http://msdl.microsoft.com/download/symbols/" + pdbName + "/" + reader.debugGUID.ToString("N").ToUpper() + reader.pdbage + "/" + pdbName;
                    else
                        downloadURL = SymbolServerUrl + "/" + pdbName + "/" + reader.debugGUID.ToString("N").ToUpper() + reader.pdbage + "/" + pdbName;
                }
            }
            return downloadURL;
        }

        public static void Main(string[] args)
        {
            try
            {
                if (args.Length < 2)
                {
                    throw new ArgumentException("Not enogth parameters");
                }

                var serverURL = args[0];
                var exePath = args[1];

                var url = BuildUrl(exePath);
                if (url == string.Empty)
                {
                    throw new Exception("Failed to build url");
                }


            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                Environment.Exit(1);
            }

        }
    }
}