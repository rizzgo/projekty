import torch
import numpy as np
import regression as fo


if __name__ == "__main__":
    params_model = torch.load("ground_truth_model/model_weights.pt")
    # params_symb = torch.load("good_results/symbolic_weights_conv1w-pca_91%.pt")
    params_symb = torch.load("good_results/symbolic_weights_conv2-pca_83%.pt")
    # print("params:", params_model.keys())
    layer_name = "conv2.weight"
    layer_true = params_model[layer_name]
    layer_pred = params_symb[layer_name]
    length = np.prod(layer_true.shape)
    shape2d = (layer_true.shape[0], layer_true.shape[1] * layer_true.shape[2] * layer_true.shape[2])
    # fo.plot_image(layer_true, layer_pred, shape2d, layer_name)
    # x = np.arange(length).reshape(-1, 1)
    # fo.plot1d(x, layer_true, layer_pred/100, f"doc_{layer_name}")
    fo.plot_image(layer_true, layer_pred, shape2d, f"doc_{layer_name}")

    # params_model = torch.load("ground_truth_model/model_weights.pt")
    # # params_symb = torch.load("good_results/symbolic_weights_conv1w-pca_91%.pt")
    # # fc2w-72%/fc2.weight22.png
    # params_symb = torch.load("good_results/symbolic_weights_fc2w-72%.pt")
    # # print("params:", params_model.keys())
    # layer_name = "fc2.weight"
    # layer_true = params_model[layer_name][22].reshape(-1, 1)
    # layer_pred = params_symb[layer_name][22].reshape(-1, 1)
    # length = np.prod(layer_true.shape)
    # # shape2d = (layer_true.shape[0] * layer_true.shape[1], layer_true.shape[2])
    # # fo.plot_image(layer_true, layer_pred, shape2d, layer_name)
    # x = np.arange(length).reshape(-1, 1)
    # fo.plot1d(x, layer_true, layer_pred/100, f"doc_{layer_name}")