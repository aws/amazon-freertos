// Convert from r_cg_vector_table.c's Reserved_Vector[] to r_cg_vector_pragma.h's R_PRAGMA_INTERRUPT()

var adTypeText = 2; // ADO StreamTypeEnum Value
var adSaveCreateOverWrite = 2; // ADO SaveOptionsEnum Value
var adWriteLine = 1; // ADO StreamWriteEnum Value
var adReadLine = -2; // ADO StreamReadEnum Value
var adLF = 10; // ADO LineSeparatorEnum Value
var adCR = 13; // ADO LineSeparatorEnum Value
var adCRLF = -1; // ADO LineSeparatorEnum Value

var fso = new ActiveXObject("Scripting.FileSystemObject");
var srcstrm = new ActiveXObject("ADODB.Stream");
var dststrm = new ActiveXObject("ADODB.Stream");

var srcfile = WScript.Arguments(0);
var dstfile = WScript.Arguments(1);

srcstrm.Type = adTypeText;
srcstrm.Charset = "_autodetect_all";
srcstrm.LineSeparator = adCRLF;
srcstrm.Open();

srcstrm.LoadFromFile(srcfile);

dststrm.Type = adTypeText;
dststrm.Charset = "ascii";
dststrm.LineSeparator = adCRLF;
dststrm.Open();

// Skip until the line of Reserved_Vector[]
while(!srcstrm.EOS)
{
    line = srcstrm.ReadText(adReadLine);

    if ((index = line.search(/^void \* const Reserved_Vector\[\] __attribute\(\(section\("\.rvectors"\)\)\) =$/)) == 0)
    {
        break;
    }
}

// Convert from Reserved_Vector[] to R_PRAGMA_INTERRUPT()
var number = 0;
while(!srcstrm.EOS)
{
    line = srcstrm.ReadText(adReadLine);

    if (line.search(/^{$/) == 0)
    {
        // start
        continue;
    }
    if (line.search(/^}$/) == 0)
    {
        // end
        break;
    }
    else if (line.search(/^ *\(void \(\*\)\(void\)\)0xFFFFFFFF,$/) == 0)
    {
        // skip
    }
    else if (line.search(/^ *undefined_interrupt_source_isr,$/) == 0)
    {
        // skip
    }
    else if (line.search(/^ *(.*),$/) == 0)
    {
        // do
        dststrm.WriteText("R_PRAGMA_INTERRUPT(" + RegExp.$1 + ", " + number + ")", adWriteLine);
    }
    else
    {
        // ignore
        continue;
    }
    number++;
}

srcstrm.Close();

dststrm.SaveToFile(dstfile, adSaveCreateOverWrite);

dststrm.Close();
