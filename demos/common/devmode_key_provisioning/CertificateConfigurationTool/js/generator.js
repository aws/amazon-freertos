
// Check for the various File API support.
if (window.File && window.FileReader && window.FileList && window.Blob) {
    // Success! All the File APIs are supported.
} else {
    alert('Please use a web browser that supports HTML5 file APIs.');
}

function formatCredentialText(credentialText) {
    // Replace any CR/LF pairs with a newline character.
    credentialText = credentialText.replace(/\r\n/g, "\n");

    // Add line endings for C-language variable declaration.
    credentialText = credentialText.replace(/\n/g, "\\n\"\n\"");

    // Remove '\n"' from the last line of the declaration and add a semicolon.
    credentialText = credentialText.slice(0, -5) + "\";\n";
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
        pemCertificateText = "static const char clientcredentialCLIENT_CERTIFICATE_PEM\[\] = \n" + "\"" +
            formatCredentialText(pemCertificateText);

        // Because this is async, read next file inline.
        readerPrivateKey.readAsText(pemInputFilePrivateKey.files[0]);
    });

    // Define a handler to create appropriate private key file text.
    readerPrivateKey.onload = (function (e) {
        pemPrivateKeyText = e.target.result;

        // Add C-language variable declaration plus EOL formatting.
        pemPrivateKeyText = "static const char clientcredentialCLIENT_PRIVATE_KEY_PEM\[\] =\n" + "\"" +
            formatCredentialText(pemPrivateKeyText);

        // Concatenate results for the final output.
        outputText = client_certificate_comment_text +
            pemCertificateText + "\n" +
            private_key_comment_text +
            pemPrivateKeyText + "\n" +
            jitr_certificate_text;

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

    private_key_comment_text =
"\/\*\n\
\ * PEM-encoded client private key.\n\
\ *\n\
\ * Must include the PEM header and footer:\n\
\ * \"-----BEGIN RSA PRIVATE KEY-----\"\n\
\ * \"...base64 data...\"\n\
\ * \"-----END RSA PRIVATE KEY-----\";\n\
\ *\/\n";

    client_certificate_comment_text =
"\/\*\n\
\ * PEM-encoded client certificate.\n\
\ *\n\
\ * Must include the PEM header and footer:\n\
\ * \"-----BEGIN CERTIFICATE-----\"\n\
\ * \"...base64 data...\"\n\
\ * \"-----END CERTIFICATE-----\";\n\
\ *\/\n";

    jitr_certificate_text =
"\/\*\n\
\ * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).\n\
\ *\n\
\ * If used, must include the PEM header and footer:\n\
\ * \"-----BEGIN CERTIFICATE-----\"\n\
\ * \"...base64 data...\"\n\
\ * \"-----END CERTIFICATE-----\";\n\
\ *\/\n\
\static const char * clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM = NULL;";

    //Start first async read - other calls are chained inline
    readerCertificate.readAsText(pemInputFileCertificate.files[0]);
}

