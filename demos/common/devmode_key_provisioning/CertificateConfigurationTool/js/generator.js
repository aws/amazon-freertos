
// Check for the various File API support.
if (window.File && window.FileReader && window.FileList && window.Blob) {
    // Success! All the File APIs are supported.
} else {
    alert('Please use a newer browser that supports HTML5 file APIs.');
}

function generateCertificateConfigurationHeader() {
    pemCertificateText = "";
    pemPrivateKeyText = "";
    filename = "aws_clientcredential_keys.h";

    var readerCertificate = new FileReader();
    var readerPrivateKey = new FileReader();

    //Define handler to create appropriate client certificate file text.
    readerCertificate.onload = (function (e) {
        var text = e.target.result;
        text = "static const char clientcredentialCLIENT_CERTIFICATE_PEM\[\] = \n" + "\"" +
            e.target.result.replace(/\n/g, "\\n\"\n\"");
			
			//eof so remove the last double quote and new line and and add semicolon
			pemCertificateText = text.slice(0,-2)  + ";";

        //Because this is async, read next file inline.
        readerPrivateKey.readAsText(pemInputFilePrivateKey.files[0]);

    });

    //Define handler to create appropriate private key file text.
    readerPrivateKey.onload = (function (e) {
        var text = e.target.result;
         text = "static const char clientcredentialCLIENT_PRIVATE_KEY_PEM\[\] =\n" +
            "\"" + e.target.result.replace(/\n/g, "\\n\"\n\"");
			
			//eof so remove the last double quote and new line and and add semicolon
			pemPrivateKeyText = text.slice(0,-2)  + ";";
			
        outputText = client_certificate_comment_text + pemCertificateText + "\n\n" + private_key_comment_text + pemPrivateKeyText + "\n";

        //Because this is async, handle download generation inline
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
\* PEM-encoded client private key.\n\
\*\n\
\* Must include the PEM header and footer:\n\
\* \"-----BEGIN RSA PRIVATE KEY-----\"\n\
\* \"...base64 data...\"\n\
\* \"-----END RSA PRIVATE KEY-----\";\n\
\*\/\n";

    client_certificate_comment_text =
"\/\*\n\
\* PEM-encoded client certificate\n\
\*\n\
\* Must include the PEM header and footer:\n\
\* \"-----BEGIN CERTIFICATE-----\"\n\
\* \"...base64 data...\"\n\
\* \"-----END CERTIFICATE-----\";\n\
\*\/\n";

    //Start first async read - other calls are chained inline
    readerCertificate.readAsText(pemInputFileCertificate.files[0]);
}

