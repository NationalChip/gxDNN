## Example of PyTorch to TensorFlow Model Conversion

### PyTorch Model Training

> python torch_mnist_train.py

Generate `mnist_cnn.pt` after training.

### PyTorch Model Inference

> python torch_mnist_inference.py

Generate `model_weights.npz` which contains TensorFlow format weights after inference.

### TensorFlow Model Inference

> python tf_mnist_inference.py

Load `model_weights.npz` and generate frozen pb.

