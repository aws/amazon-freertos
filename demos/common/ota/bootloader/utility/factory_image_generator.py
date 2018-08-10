import argparse
import sys
from OpenSSL import crypto
import os

from ota_image_generator import validateFilePath \
    , appendOTADescriptor \
    , toLitteEndianByte \
    , format32BitHexStr \
    , printAddOTADescriptorResult \
    , getFileSize

HEX_START = "0x1D000000"
EXCLUDE_START = "0x1D000240"
EXCLUDE_END = "0x1D0004A0"


def printFactoryImageStruct(processedImagePath, trailerSize, numLinesImageContent, descripFixedSize):
    """
    print structure of factory image

    :param processedImagePath: path of processed image
    :param trailerSize: size of trailer in bytes
    :param numLinesImageContent: number of lines of image content to output
    :param descripFixedSize: fixed size in bytes for storing signature type description
    :return:
    """

    cuttingLine = "--------"
    subCuttingLine = "-----"


    #print magic code
    with open(processedImagePath,"rb") as f:
        print(cuttingLine + subCuttingLine + "Magic Code" + subCuttingLine + cuttingLine)
        byte = f.read(8)
        print(byte)


    # print [ota_descriptor + image content]
    printAddOTADescriptorResult(processedImagePath, numLinesImageContent,8)


    # print trailer
    fSize = getFileSize(processedImagePath)
    f = open(processedImagePath, "rb")
    f.seek(fSize - trailerSize)

    print(cuttingLine + cuttingLine + "Trailer" + cuttingLine + cuttingLine)

    try:
        print(subCuttingLine + " signature type " + subCuttingLine)
        byte = f.read(descripFixedSize)
        print(byte)

        print(subCuttingLine + " signature size " + subCuttingLine)
        byte = f.read(4)
        print(format32BitHexStr(hex(int.from_bytes(byte, "little"))))

        byte = f.read()
        print(subCuttingLine + " signature " + subCuttingLine)
        print(byte)

    finally:
        f.close()


def parseCMDParam():
    """
    parse input image path and hardware platform name from command line.

    raise exception if these parameters are not provided by user.

    :return: input image path, hardware platform name and private key path
    """

    progName = sys.argv[0]

    format = "python3 " + progName + " [-h] -b binary_path -p hardware_platform -k private_key_path -x bootloader_hex_file_path"

    example1 = "\t get help: \n" + "\t\tpython3 " + progName + " -h"

    example2 = "\t use folder/inputImage.bin, MCHP-Curiosity-PIC32MZEF, private_key.pem , aws_bootloader.X.production.hex as parameter : \n" \
               + "\t\tpython3 " + progName + " -b folder/inputImage.bin -p MCHP-Curiosity-PIC32MZEF -k private_key.pem -x aws_bootloader.X.production.hex"

    usageMsg = format + "\n\n" + "example usages:" + "\n" + example1 + "\n" + example2

    parser = argparse.ArgumentParser(usage=usageMsg)

    parser.add_argument('-b', required=True, help=" path of input binary image ")
    parser.add_argument('-p', required=True, help=" hardware platform name ")
    parser.add_argument('-k', required=True, help=" path of the private key used to sign the image ")
    parser.add_argument('-x', required=True, help=" path of the bootloader hex file ")

    args = vars(parser.parse_args())

    inputImagePath = args["b"]

    hardwarePlatform = args["p"]

    privateKeyPath = args["k"]

    bootloaderHexPath = args["x"]

    return inputImagePath, hardwarePlatform, privateKeyPath, bootloaderHexPath


def getSignitureLocally(pathOfImageToSign, privateKeyPath, digestMethod):
    """
    sign the given image by using openssl

    :param pathOfImageToSign:
    :param privateKeyPath:
    :param digestMethod:
    :return:
    """

    with open(privateKeyPath, "rb") as f:
        pKeyBuffer = f.read()

    pKey = crypto.load_privatekey(crypto.FILETYPE_PEM, pKeyBuffer)

    with open(pathOfImageToSign, "rb") as f:
        imageBuffer = f.read()

    signature = crypto.sign(pkey=pKey, data=imageBuffer, digest=digestMethod)

    return signature


def getTrailer(signature, sigTypeDescrip, descripFixedSize, sigFixedSize):
    """
    create a trailer which has the following format
    [a + b + c], where

    a = signature type description
    (Use fixed size to store it. Use zero to fill up the rest if description size is smaller than fixed size)

    b = size of signature
    (converted into hexadecimal representation in 32 bits in little endian format)

    c = signature
    (Used fixed size to store it. Use zero to fill up the rest if signature size is smaller than fixed size)

    :param signature:
    :param sigTypeDescrip: signature type description
    :param descripFixedSize: fixed size in bytes for storing signature type description
    :param sigFixedSize: fixed size in bytes for storing signature
    :return:
    """
    trailer = bytearray()

    # append signature type description  to fixed
    sigTypeDescrip = bytearray(source=sigTypeDescrip, encoding="utf-8")
    typeDescripSize = len(sigTypeDescrip)
    if typeDescripSize > descripFixedSize:
        raise Exception("Signature type description exceeded allowed size! Description size: " + sigTypeDescrip
                        + ". Allowed maximum size: " + descripFixedSize
                        + ". Description given: " + sigTypeDescrip)

    padSize = descripFixedSize - typeDescripSize
    pad = bytearray(padSize)
    trailer.extend(sigTypeDescrip)
    trailer.extend(pad)

    # append signature size as a 4-byte filed in little endian format
    sigSize = len(signature)
    sigSizeFiled = hex(sigSize)
    sigSizeFiled = format32BitHexStr(sigSizeFiled)
    sigSizeFiled = toLitteEndianByte(sigSizeFiled)
    trailer.extend(sigSizeFiled)

    # append signature
    if len(signature) > sigFixedSize:
        raise Exception("Signature size  exceeded allowed size! signature size: " + sigSize
                        + ". Allowed maximum size: " + sigFixedSize)
    padSize = sigFixedSize - sigSize
    pad = bytearray(padSize)
    trailer.extend(signature)
    trailer.extend(pad)

    return trailer


def appendTrailer(inputImagePath, trailer, outputPath):
    """
    append trailer to the given image

    :param inputImagePath:
    :param trailer:
    :param outputPath:
    :return:
    """

    with open(inputImagePath, "rb") as f:
        inputContent = f.read()

    with open(outputPath, "wb") as f:
        f.write(inputContent)
        f.write(trailer)



def addFactoryMagicCode(inputImagePath,outputPath):
    """
    add 8-byte magic code
    :param inputImagePath:
    :param outputPath:
    :return:
    """
    with open(inputImagePath,"rb") as f:
        inputContent = f.read()

    magicCode = bytearray("@AFRTOS".encode('ASCII'))

    # end byte is 0xFC
    endByte = bytes.fromhex("FC")

    magicCode.extend(endByte)

    with open(outputPath,"wb") as f:
        f.write(magicCode)
        f.write(inputContent)



def convertToHex(inputImagePath, outputPath,bootLoaderHexPath):
    """
    convert given image into intel hex format.
    Newly generated file will be saved at outputPath.

    :param inputImagePath:
    :param outputPath:
    :param bootLoaderHexPath
    :return:
    """
    temp_hex = "temp.hex"
    cmd_convert = "srec_cat " + inputImagePath + " -binary -offset " + HEX_START + " -o " + temp_hex + " -Intel "
    os.system(cmd_convert)

    cmd_unify = "srec_cat " + temp_hex +" -intel "+ bootLoaderHexPath + " -intel -o " + outputPath + " -intel"
    os.system(cmd_unify)

    os.system("rm " + temp_hex)



if __name__ == "__main__":
    inputImagePath, hardwarePlatform, privateKeyPath, bootloaderHexPath = parseCMDParam()

    #verify parameter
    validateFilePath(inputImagePath)
    validateFilePath(privateKeyPath)
    validateFilePath(bootloaderHexPath)

    #get ota image
    otaImagePath = appendOTADescriptor(inputImagePath, hardwarePlatform)


    #sign the ota image
    digestMethod = "sha256"  # use "sha256" method
    signature = getSignitureLocally(otaImagePath, privateKeyPath, digestMethod)

    typeStr = "sig-sha256-ecdsa"
    descripFixedSize = 32  # signature description is fixed to 32 bytes, will use zeroes to fill up the rest
    sigFixedSize = 256  # signature is fixed to 256 bytes, will use zeroes to fill up the rest
    trailer = getTrailer(signature, typeStr, descripFixedSize, 256)

    #append trailer to ota image
    print("\nAppending trailer to image " + otaImagePath + " ...")
    factoryImagePath = otaImagePath.replace(".ota.bin", ".initial.bin")
    appendTrailer(inputImagePath=otaImagePath, trailer=trailer, outputPath=factoryImagePath)
    print("\nTrailer appended! factory image generated at " + factoryImagePath)

    # add factory magic code
    print("\nAdding magic code to " + factoryImagePath + "  ...\n")
    addFactoryMagicCode(inputImagePath=factoryImagePath, outputPath= factoryImagePath)


    #print structure
    print("\nStructure of factory image : "+ factoryImagePath
          +"\n[magic_code + ota_descriptor + image_content + trailer]:\n")
    printFactoryImageStruct(processedImagePath=factoryImagePath, trailerSize=len(trailer), numLinesImageContent=10,
                            descripFixedSize=descripFixedSize)


    #convert factory image into hex format
    print("\nConverting factory image " + factoryImagePath + " into hex format ....\n")
    hexFilePath = factoryImagePath.replace("initial.bin", "unified.factory.hex")
    convertToHex(inputImagePath=factoryImagePath, outputPath=hexFilePath,bootLoaderHexPath=bootloaderHexPath)
    print("\nHex file is generated at " + hexFilePath)

    print("\nHex file info:")
    os.system("srec_info " + hexFilePath + " -Intel ")
