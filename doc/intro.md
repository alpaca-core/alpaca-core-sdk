# AC Local: Introduction

The Alpaca Core Local SDK, or *AC Local* for short, is a multi-platform SDK for local AI Inference.

"Local" in this context means running on the device which executes the code. This could be a server, a desktop, or a mobile device.

AC Local provides a unified API for doing inference with [multiple models](https://alpacacore.com/docs/getting-started/models).

## API Elements

The API defines the following elements:

```mermaid
flowchart LR
  Loader --loads--> Model --creates--> Instances
  Instance --runs-->Op
  Op -.changes state.-> Instance 
  Op --produces--> Result  
```

### Model Loader

A `ModelLoader` is an object which can load a model from a description. The description is a JSON object which contains the model type, assets, and other parameters. The loader is a factory for models.

### Model

A `Model`, in API terms, is an object which represents an AI model (weights, parameters) loaded into memory. Once created, a model is immutable and *does* nothing on its own, but is the means to create an...

### Instance

The `Instance` is an object associated with a `Model` which can do inference based on the parameters it's created with. The instance holds a private state which is not shared with other instances (what *is* shared is the model). The instance state is not immutable and can change with each subsequent inference operation.

### Instance Operation

... or `op` for short, is a function (method) which can be called on an instance to perform inference and return a result. Ops may change the internal instance state. 

Running can be visualized as sequence diagram

```mermaid
sequenceDiagram
    participant App
    create participant Instance    
    App ->> Instance : create
    App ->> Instance : run op
    create participant Op
    Instance ->> Op : execute
    Op -->> Instance : change state
    destroy Op
    Op ->> App : produce result
```

### Example

Now, this is all pretty abstract, so let's give an example. In pseudo-code:

```python
model = loader.load("llama-2-7b") # create a model
instance = model.create_instance() # create an instance
result = instance.complete("A recipe for rice cakes:") # run op and get result
print(result) # consume the result
```

## API Layers

The example above is pretty neat, but our goal is to have a *unified* API for multiple models. There's nothing unified in calling `.complete("text")` for an instance. Such an operation simply makes no sense for many types of models.

To facilitate the goal the API is split into two layers:

### Inference API

This is what's different for each model type.

Some close (but not quite complete) descriptions of it could be duck-typed, or "stringly"-typed, or JSON-typed.

Every model type defines a schema for the inference API. The schema describes things like what types of instances can be created for the model, what ops each instance provides, then what input each op gets and what it returns as a result. A more detailed description of schemas (or the schema schema) is available [here](iapi.md).

The main carrier of data for this API is an object called `Dict`. This stands for dictionary. A more formal description if `Dict` is available [here](dict.md). In short it's basically a POJO (where J stands for JavaScript), so a JSON object, but with the notable addition of the data type `binary` which is contiguous memory buffer. So... not a JSON, but a [CBOR](https://cbor.io/) object, at least in terms of data types.

With all this we can transform our example from above to something like *(still pseudo-code)*:

```python
# create a model
model = loader.load("llama-2-7b") # load an llm

# create a general instance with a small context size
instance = model.create_instance("general", dict(context_size = 1024))

# run the op "complete" with a prompt and limit the output (lest our model goes haywire)
result = instance.run_op("complete", dict(prompt = "A recipe for rice cakes:", max_tokens = 2000))

# consume the result
recipe = result["text"]
print(recipe)
```

...but now we can also do:

```python
# create a model
model = loader.load("stable-diffusion-3") # load an image generation model

# create an instance with a specific resolution
instance = model.create_instance("general", dict(resolution = 512))

# run the op "generate" with a prompt
result = instance.run_op("generate", dict(prompt = "A 17th century oil on canvas portrait of Darth Vader"))

# consume the result
image_bytes = result["image"]
img = Image.open(io.BytesIO(image_bytes))
img.show()
```

To make this pseudo-code not-so-pseudo, we need the...

### Language API

This is the code one writes to call the Inference API from a given programming language.

Here's a quip:

> The Inference API is different for each model type and the same for all programming languages. The Language API is the same for all model types and different for each programming language.

It's what gives you the concrete representations of `Model`, `Instance`, `op`-s, and `Dict`, and most importantly a way to load models. 

The base implementation is in C++, but wrappers for other languages are provided. Find the documentation [here](lapi.md).

And with it we can have actual working code like:

```cpp
// load all plugins and the loaders that they provide
ac::local::Lib::loadAllPlugins(); 

// create a model from the first loader which accepts "llama.cpp gguf"
auto model = ac::local::Lib::createModel(
    {
        .type = "llama.cpp gguf",
        .assets = {
            {.path = "/path/to/model.gguf"}
        }
    }, 
    { /*default params*/ }
);

// create an instance of the model
auto instance = model->createInstance("general", { /*default params*/ });

// run the op "run" with a prompt
auto result = instance->runOp("run", {{"prompt", "A recipe for rice cakes:"}});

std::cout << result << "\n";
```

Plugins? What are plugins?

AC Local doesn't have any loaders, and thus doesn't support any models on its own. The typical way to get loaders is to use plugins which provide them. AC Local contains the tools for building and loading plugins. Read more about loaders and plugins [here](loaders-plugins.md).

Alpaca Core itself develops a few plugins.

## More

This introduction is more or less language agnostic. You can check out the C++-centric documentation on structure and internals [here](internals.md).
