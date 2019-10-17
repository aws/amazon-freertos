
// Check for the various File API support.
if (window.File && window.FileReader && window.FileList && window.Blob) {
    // Success! All the File APIs are supported.
} else {
    alert('Please use a web browser that supports HTML5 file APIs.');
}


function formatCredentialTextForHTML(credentialText) {
    // Remove whitespace at end of PEM file (to eliminate differences between input files)
    credentialText = credentialText.trim();

    // Replace any CR/LF pairs with a newline character.
    credentialText = credentialText.replace(/\r\n/g, "\n");

    // Add line endings, string quote, and line continuation, as well as html line breaks. (\n"\ <break>)
    credentialText = credentialText.replace(/\n/g, "\\n\"\\<br>\n\"");

    // Format the end of the last line with " (but no line continuation).
    credentialText = credentialText + "\"\n";

    return credentialText;
}

function generatePemTextToDisplay() {
    var readerPemFile = new FileReader()

    // Define a handler to create appropriate private key file text.
    readerPemFile.onload = (function (e) {
        pemCredentialText = e.target.result;

        // Add C-language variable declaration plus EOL formatting.
        pemCredentialText = "\"" + formatCredentialTextForHTML(pemCredentialText);

        // Give the HTML a way to access the text to display.
        document.getElementById("pemToDisplay").innerHTML = pemCredentialText;
    });

    //Start first async read - other calls are chained inline
    readerPemFile.readAsText(pemInputFile.files[0]);
}

function formatCredentialTextForHeader(credentialText) {
    // Replace any CR/LF pairs with a newline character.
    credentialText = credentialText.replace(/\r\n/g, "\n");

    // Add line endings for C-language variable declaration.
    credentialText = credentialText.replace(/\n/g, "\\n\"\\\n\"");

    // Remove '\n"' from the last line of the declaration and add a semicolon.
    credentialText = credentialText.slice(0, -6) + "\"\n";
    return credentialText;
}

function generateCertificateConfigurationHeader() {
    pemCertificateText = "";
    pemPrivateKeyText = "";
    filename = "aws_clientcredential_keys.h";

    var readerCertificate = new FileReader();
    var readerPrivateKey = new FileReader();

    // Define a handler to create appropriate client certificate file text.
    readerCertificate.onload = (function (e) {
        pemCertificateText = e.target.result;

        // Add C-language variable declaration plus EOL formatting.
        pemCertificateText = "#define keyCLIENT_CERTIFICATE_PEM \\\n" + "\"" +
            formatCredentialTextForHeader(pemCertificateText);

        // Because this is async, read next file inline.
        readerPrivateKey.readAsText(pemInputFilePrivateKey.files[0]);
    });

    // Define a handler to create appropriate private key file text.
    readerPrivateKey.onload = (function (e) {
        pemPrivateKeyText = e.target.result;

        // Add C-language variable declaration plus EOL formatting.
        pemPrivateKeyText = "#define keyCLIENT_PRIVATE_KEY_PEM \\\n" + "\"" +
            formatCredentialTextForHeader(pemPrivateKeyText);

        // Concatenate results for the final output.
        outputText = header_begin_text +
            client_certificate_comment_text +
            pemCertificateText + "\n" +
            private_key_comment_text +
            pemPrivateKeyText + "\n" +
            jitr_certificate_text +
            header_end_text;

        // Because this is async, handle download generation inline.
        var downloadBlob = new Blob([outputText], { type: 'text/plain' });
        if (window.navigator.msSaveOrOpenBlob) {
            window.navigator.msSaveBlob(downloadBlob, filename);
        } else {
            var downloadLink = document.createElement('a');
            downloadLink.href = window.URL.createObjectURL(downloadBlob);
            downloadLink.download = filename;
            document.body.appendChild(downloadLink);
            downloadLink.click();
            document.body.removeChild(downloadLink);
        }
    });

    header_begin_text = 
"#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H\n\
#define AWS_CLIENT_CREDENTIAL_KEYS_H\n\
\n\
#include \<stdint.h\>\n\
\n";

    private_key_comment_text =
"\/\*\n\
\ * PEM-encoded client private key.\n\
\ *\n\
\ * Must include the PEM header and footer:\n\
\ * \"-----BEGIN RSA PRIVATE KEY-----\\n\"\\\n\
\ * \"...base64 data...\\n\"\\\n\
\ * \"-----END RSA PRIVATE KEY-----\"\n\
\ *\/\n";

    client_certificate_comment_text =
"\/\*\n\
\ * PEM-encoded client certificate.\n\
\ *\n\
\ * Must include the PEM header and footer:\n\
\ * \"-----BEGIN CERTIFICATE-----\\n\"\\\n\
\ * \"...base64 data...\\n\"\\\n\
\ * \"-----END CERTIFICATE-----\"\n\
\ *\/\n";

    jitr_certificate_text =
"\/\*\n\
\ * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).\n\
\ *\n\
\ * If used, must include the PEM header and footer:\n\
\ * \"-----BEGIN CERTIFICATE-----\\n\"\\\n\
\ * \"...base64 data...\\n\"\\\n\
\ * \"-----END CERTIFICATE-----\"\n\
\ *\/\n\
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  \"\"\n\
\n";

    header_end_text =
"\n\
#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H *\/\n";

    //Start first async read - other calls are chained inline
    readerCertificate.readAsText(pemInputFileCertificate.files[0]);
}

