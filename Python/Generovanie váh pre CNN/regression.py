import torch
import numpy as np
import matplotlib.pyplot as plt
from gplearn.genetic import SymbolicRegressor
from sklearn.decomposition import PCA

def plot1d(x_test, y_true, y_pred, layer_name):
    plt.figure(figsize=(12.8, 7.2), dpi=100)
    plt.rc('font', size=16)
    plt.plot(x_test, y_true, label='pôvodné hodnoty')
    plt.plot(x_test, y_pred, label='generované hodnoty')
    plt.xlabel("parametre")
    plt.ylabel("hodnoty")
    plt.legend()
    plt.savefig(f"{layer_name}.png")

def plot_image(y_true, y_pred, im_shape, layer_name):
    vmin, vmax = y_true.min(), y_true.max()
    fig, ax = plt.subplots(nrows=1, ncols=3)

    im0 = ax[0].imshow(y_true.reshape(im_shape), cmap='gray', vmin=vmin, vmax=vmax)
    ax[0].axis('off')

    im1 = ax[1].imshow(y_pred.reshape(im_shape), cmap='gray', vmin=vmin, vmax=vmax)
    ax[1].axis('off')

    im2 = ax[2].imshow((y_pred.reshape(im_shape)-y_true.reshape(im_shape)), cmap='gray', vmin=vmin, vmax=vmax)
    ax[2].axis('off')

    fig.colorbar(im0,ax=ax[0])
    fig.colorbar(im1,ax=ax[1])
    fig.colorbar(im2,ax=ax[2])
    # fig.colorbar(axs[0].imshow(img1), ax=axs[0])
    fig.savefig(f"{layer_name}.pdf", format="pdf")

def __optimize(x, y, data_name):
    est_gp = SymbolicRegressor(population_size=2000, tournament_size=200,
                            generations=50, stopping_criteria=0.01,
                            const_range=(-1.0, 1.0), metric='mse',
                            function_set=('add', 'sub', 'mul', 'div', 'sin', 'cos'),
                            verbose=1)
    est_gp.fit(x, y)
    print(est_gp._program)
    with open(f"{data_name}.py", "w") as f:
        f.write(f"def {data_name}(x):\n")
        f.write("    return " + str(est_gp._program))

    return torch.tensor(est_gp.predict(x))

def optimize_layer(layer, layer_name):    
    x = np.array([np.array(idx) for idx in np.ndindex(layer.shape)])
    y_true = layer.ravel()*1000
    print(x)

    y_pred = __optimize(x, y_true, layer_name)
    
    plot_image(y_true, y_pred, layer.shape, layer_name)
    
    return y_pred.reshape(layer.shape)

def optimize_filter(filter, filter_name):
    shape2d = (filter.shape[0] * filter.shape[1], filter.shape[2])
    
    x = np.array([np.array(idx) for idx in np.ndindex(shape2d)])
    y_true = filter.ravel()*1000
    print(x)

    y_pred = __optimize(x, y_true, filter_name)
    
    plot_image(y_true, y_pred, shape2d, filter_name)
    
    return y_pred.reshape(filter.shape)

def optimize_row(filter, filter_name):
    length = np.prod(filter.shape)
    
    x = np.arange(length).reshape(-1, 1)
    y_true = filter.ravel()*100
    print(x)

    y_pred = __optimize(x, y_true, filter_name)
    
    plot1d(x, y_true, y_pred, filter_name)
    print(y_pred.reshape(filter.shape))

    return y_pred.reshape(filter.shape)


def optimize_pca(layer, layer_name, n_components=None, layer_shape=None):
    if layer_shape == None:
        layer_shape = layer.shape
    if n_components == None:
        n_components = layer_shape[0]
    pca = PCA(n_components=n_components)
    pca_layer = pca.fit_transform(params[layer_name].reshape(layer_shape))
    pca_layer = optimize_layer(pca_layer, f"pca_{layer_name}")
    return torch.tensor(pca.inverse_transform(pca_layer)).reshape(layer.shape)

if __name__ == "__main__":
    params = torch.load("model_weights.pt")
    folder = "slices"
    layer_name = "conv2.weight"

    # Whole layer optimization
    # for layer_name in params.keys():
    #     params[layer_name] = optimize_layer(params[layer_name], layer_name)

    # Group optimization of one layer - need to choose folder and layer_name 
    optimizer = optimize_filter if layer_name.startswith("conv") and layer_name.endswith("weight") else optimize_row
    params[layer_name] = torch.stack([optimizer(filter, f"{folder}/{layer_name}{i}") for i, filter in enumerate(params[layer_name])])

    # PCA layer optimization - need to choose layer_name
    # layer = params[layer_name]
    # shape2d = (layer.shape[0], layer.shape[1] * layer.shape[2] * layer.shape[3])
    # params[layer_name] = optimize_pca(layer, layer_name, n_components=16, layer_shape=shape2d)

    torch.save(params, 'symbolic_weights.pt')
