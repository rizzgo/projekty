import torch
import torch.nn as nn
from torchsummary import summary
from lenet import LeNet, val_loader

model = LeNet()
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
model = model.to(device)

params = torch.load('symbolic_weights.pt')
model.load_state_dict(params)

epochs = 3
criterion = nn.CrossEntropyLoss()

def eval():
    for i in range(epochs):
        acc = []
        losses = []
        model.eval()
        for data in val_loader:
            input, target = data
            output = model(input)
            loss = criterion(output, target)

            with torch.no_grad():
                predicted = torch.argmax(output, dim=1)
                acc.append((predicted == target).sum().item()/len(target))
                losses.append(loss)
        print(f"{i}. val, acc: {sum(acc)/len(acc)}, loss: {sum(losses)/len(losses)}")

if __name__ == '__main__':
    summary(model, (1,28,28))
    eval()
