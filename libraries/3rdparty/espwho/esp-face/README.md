# ESP-Face

This is a component which provides functions of face detection and face recognition, and also operations of neurual network.

It can be used as a component of some project as it doesn't support any interface of periphrals.

By default, it works along with ESP-WHO, which is a project-level repository.

## Face detection

It can detect a human face with the model MTMN.

The input should be an image with the format of RGB 24-bits.

The output is the human face coordinates if there is one in the image.

- left top
- right down
- landmarks

More details are [HERE](face_detection/README.md).

## Face recognition

If one human face is detected through the process above, it can be verified with the faces enrolled before.

The input is the original image and the results of face detection process.

The output is a 512-d vector that represents the face. Then through comparing with the existing face vectors, you can determine whether the two faces are from a same person.

More details are [HERE](face_recognition/README.md).

## Deep learning library

All neural network operations are defined in this library. They are and only are utilized for neural network.

The library contains basic matrix operations, basic deep neural network operations, network structures, and coefficients.

More details are [HERE](lib/README.md)

