using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace WindowsApplication1
{
	public partial class Form1 : Form
	{
		const int EVERYTHING_OK	= 0;
		const int EVERYTHING_ERROR_MEMORY = 1;
		const int EVERYTHING_ERROR_IPC = 2;
		const int EVERYTHING_ERROR_REGISTERCLASSEX = 3;
		const int EVERYTHING_ERROR_CREATEWINDOW = 4;
		const int EVERYTHING_ERROR_CREATETHREAD = 5;
		const int EVERYTHING_ERROR_INVALIDINDEX = 6;
		const int EVERYTHING_ERROR_INVALIDCALL = 7;

        [DllImport("Everything32.dll", CharSet = CharSet.Unicode)]
		public static extern int Everything_SetSearchW(string lpSearchString);
		[DllImport("Everything32.dll")]
		public static extern void Everything_SetMatchPath(bool bEnable);
		[DllImport("Everything32.dll")]
		public static extern void Everything_SetMatchCase(bool bEnable);
		[DllImport("Everything32.dll")]
		public static extern void Everything_SetMatchWholeWord(bool bEnable);
		[DllImport("Everything32.dll")]
		public static extern void Everything_SetRegex(bool bEnable);
		[DllImport("Everything32.dll")]
		public static extern void Everything_SetMax(int dwMax);
		[DllImport("Everything32.dll")]
		public static extern void Everything_SetOffset(int dwOffset);

		[DllImport("Everything32.dll")]
		public static extern bool Everything_GetMatchPath();
		[DllImport("Everything32.dll")]
		public static extern bool Everything_GetMatchCase();
		[DllImport("Everything32.dll")]
		public static extern bool Everything_GetMatchWholeWord();
		[DllImport("Everything32.dll")]
		public static extern bool Everything_GetRegex();
		[DllImport("Everything32.dll")]
		public static extern UInt32 Everything_GetMax();
		[DllImport("Everything32.dll")]
		public static extern UInt32 Everything_GetOffset();
		[DllImport("Everything32.dll")]
		public static extern string Everything_GetSearchW();
		[DllImport("Everything32.dll")]
		public static extern int Everything_GetLastError();

		[DllImport("Everything32.dll")]
		public static extern bool Everything_QueryW(bool bWait);

		[DllImport("Everything32.dll")]
		public static extern void Everything_SortResultsByPath();

		[DllImport("Everything32.dll")]
		public static extern int Everything_GetNumFileResults();
		[DllImport("Everything32.dll")]
		public static extern int Everything_GetNumFolderResults();
		[DllImport("Everything32.dll")]
		public static extern int Everything_GetNumResults();
		[DllImport("Everything32.dll")]
		public static extern int Everything_GetTotFileResults();
		[DllImport("Everything32.dll")]
		public static extern int Everything_GetTotFolderResults();
		[DllImport("Everything32.dll")]
		public static extern int Everything_GetTotResults();
		[DllImport("Everything32.dll")]
		public static extern bool Everything_IsVolumeResult(int nIndex);
		[DllImport("Everything32.dll")]
		public static extern bool Everything_IsFolderResult(int nIndex);
		[DllImport("Everything32.dll")]
		public static extern bool Everything_IsFileResult(int nIndex);
        [DllImport("Everything32.dll", CharSet = CharSet.Unicode)]
        public static extern void Everything_GetResultFullPathNameW(int nIndex, StringBuilder lpString, int nMaxCount);
		[DllImport("Everything32.dll")]
		public static extern void Everything_Reset();

		public Form1()
		{
			InitializeComponent();
		}

		private void button1_Click(object sender, EventArgs e)
		{
			int i;
			const int bufsize = 260; 
			StringBuilder buf = new StringBuilder(bufsize);

			// set the search
			Everything_SetSearchW(textBox1.Text);

			// use our own custom scrollbar... 			
			// Everything_SetMax(listBox1.ClientRectangle.Height / listBox1.ItemHeight);
			// Everything_SetOffset(VerticalScrollBarPosition...);
			
			// execute the query
			Everything_QueryW(true);

			// sort by path
			// Everything_SortResultsByPath();

			// clear the old list of results			
			listBox1.Items.Clear();

			// set the window title
			Text = textBox1.Text + " - " + Everything_GetNumResults() + " Results";
			
			// loop through the results, adding each result to the listbox.
			for (i = 0; i < Everything_GetNumResults(); i++)
			{
				// get the result's full path and file name.
                Everything_GetResultFullPathNameW(i, buf, bufsize);

				// add it to the list box				
				listBox1.Items.Insert(i,buf);
			}
		}
	}
}