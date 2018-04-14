# Project 4 -- Get My Music (GMM)
David Brakman, Michael Dombrowski, Shiv Toolsidass

## Protocol
Messages exchanged over the network will be strings adhering to a limited subset of JSON (RFC 7159). We chose this higher-level message format, instead of a more concise custom format like the one in the specification for Project 3, with the goal of simplifying development, a more pressing concern than minimizing bandwidth usage. Instead of using bit manipulation, we can use a map data structure to build and parse messages according to human-readable keys.

All messages in our protocol will share a common envelope:
```JSON
{
  "version": <Integer>,
  "type": <String>,
}
```

* Version: An integer *i* corresponding to the version of the message format being sent. If a message format breaks compatibility with existing implementations, its version number should be incremented so that communicating parties can recognize the issue and either adapt or abort. The development version articulated here is version 1.

* Type: the type of the message being sent, identified by one of the following strings: "listRequest", "listResponse", "pullRequest", "pullResponse", or "leave". This type info informs the recipient what key-value pairs can be expected in the rest of the message.
  - **listRequest:** no additional information.   
  Example:
    ```
    {
      "version": 1,
      "type": "listRequest",
    }
    ```
  - **listResponse:** response info consisting of a list of (filename, checksum) tuples.  
  Example:
    ```
    {
      "version": 1,
      "type": "listResponse",
      "response": [
        {
          "filename": "file1",
          "checksum": "aaabaaajss",
        },
        {
          "filename": "file2",
          "checksum": "wg2gnxgrrr",
        }
      ]
    }
    ```
  - **pullRequest:** request info consisting of a list of (filename, checksum) tuples.  
  Example:
    ```
    {
      "version": 1,
      "type": "pullRequest",
      "request": [
        {
          "filename": "file1",
          "checksum": "aaabaaajss",
        },
        {
          "filename": "file2",
          "checksum": "wg2gnxgrrr",
        }
      ]
    }
    ```

  - **pullResponse:** response info consisting of a list of (filename, checksum, data) tuples. To be sent as a string, file data will be encoded in base64. If a pullRequest includes invalid (filename, checksum) pairs that do not exist on the server, the pullResponse will send only the files corresponding to valid (filename, checksum) pairs.
  Example:  
    ```
    {
      "version": 1,
      "type": "pullResponse",
      "response": [
        {
          "filename": "file1",
          "checksum": "aaabaaajss",
          "data": "bGFsYWxhIGhhcHB5IHN0cmluZw=="
        },
        {
          "filename": "file2",
          "checksum": "wg2gnxgrrr",
          "data": "z/rt/gcAAAEDAACAAgAAABAAAACwBQAAhQAgAAAAAAAZAAAASAAA"
        }
      ]
    }
    ```

  - **pushRequest:** request info consisting of a list of (filename, checksum, data) tuples. To be sent as a string, file data will be encoded in base64.  
  Example:
    ```
    {
      "version": 1,
      "type": "pushRequest",
      "request": [
        {
          "filename": "file3",
          "checksum": "AATTVVV",
          "data": "bGFsYWxhIGhhcHB5IHN0cmluZw=="
        },
        {
          "filename": "file4",
          "checksum": "ABBBBBBK",
          "data": "z/rt/gcAAAEDAACAAgAAABAAAACwBQAAhQAgAAAAAAAZAAAASAAA"
        }
      ]
    }
    ```

  - **pushResponse:** response info consisting of a list of (filename, checksum) pairs of the files written to the server. If a pushRequest includes checksums that correspond to files already existing on the server, then those files will not be overwritten, and the pushResponse will not list those files.  
  Example:
    ```
    {
      "version": 1,
      "type": "pushResponse",
      "response": [
        {
          "filename": "file3",
          "checksum": "AATTVVV",
        },
        {
          "filename": "file4",
          "checksum": "ABBBBBBK",
        }
      ]
    }
    ```

  - **leave:** no additional information.  
  Example:  
    ```
    {
      "version": 1,
      "type": "leave",
    }
    ```



## Client

## Server

### Multithreading
