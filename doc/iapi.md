# Inference API

The inference API defines the communication protocol with a given inference engine. It is, thus, potentially different for each model type. The unifying factor is the API schema. An inference engine must produce a schema which describes its capabilities.

## Schema

(or, you know, schema schema)

```json
{
  "description": "A schema for the inference API of a model type",
  "type": "object",
  "properties": {
    "id": {
      "description": "A unique identifier of the schema",
      "type": "string"
    },
    "description": {
      "description": "A human-readable description of the inference engine",
      "type": "string"
    },
    "params": {
      "description": "A JSON-schema of parameters that can be used to create a model",
      "type": "json-schema"
    }
    "isntances": {
      "description": "A mapping of instance name to instance schema",
      "type": "object",
      "additionalProperties": {
        "description": "A schema for an instance",
        "type": "object",
        "properties": {
          "description": {
            "description": "A human-readable description of the instance",
            "type": "string"
          },
          "params": {
            "description": "A schema for the initialization parameters of the instance",
            "type": "json-schema"
          },
          "ops": {
            "description": "A mapping of op name to op schema",
            "type": "object",
            "additionalProperties": {
              "description": "A schema for an op",
              "type": "object",
              "properties": {
                "description": {
                  "description": "A human-readable description of the op",
                  "type": "string"
                },
                "params": {
                  "description": "A schema for the input of the op",
                  "type": "json-schema"
                },
                "return": {
                  "description": "A schema for the output of the op",
                  "type": "json-schema"
                }
              }
            }
          }
        }
      }
    }
  }
}
```

## Example: 

As an example, here's how the whisper.cpp schema looked like at some point during development:

```json
{
  "id": "whisper",
  "description": "Inference based on our fork of https://github.com/ggerganov/whisper.cpp",
  "params": { "type": "null" },
  "instances": {
    "general": {
      "description": "General instance",
      "params": { "type": "null" },
      "ops": {
        "transcribe": {
          "description": "Run the llama.cpp inference and produce some output",
          "params": {
            "type": "object",
            "properties": {
              "audioBinaryMono": {
                "description": "Audio data to transcribe",
                "type": "binary"
              }
            },
            "required": [ "audioBinaryMono" ]
          },
          "return": {
            "type": "object",
            "properties": {
              "result": {
                "description": "Transcription of audio",
                "type": "string"
              }
            },
            "required": [ "result" ]
          }
        }
      }
    }
  }
}
```

## Supported Models

The supported models and their schemas can be found [here](https://alpacacore.com/docs/getting-started/models)

