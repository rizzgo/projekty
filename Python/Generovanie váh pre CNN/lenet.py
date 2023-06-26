import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision import datasets
from torchvision.transforms import ToTensor
from torch.utils.data import DataLoader
from torchsummary import summary

class LeNet(nn.Module):
    def __init__(self):
        super(LeNet, self).__init__()
        self.conv1 = nn.Conv2d(1, 6, 5)
        self.conv2 = nn.Conv2d(6, 16, 5)
        self.fc1 = nn.Linear(256, 120)
        self.fc2 = nn.Linear(120, 84)
        self.fc3 = nn.Linear(84, 10)

    def forward(self, x):
        x = F.max_pool2d(F.relu(self.conv1(x)), (2,2))
        x = F.max_pool2d(F.relu(self.conv2(x)), 2)
        x = torch.flatten(x,1)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x
    
train_dataset = datasets.MNIST(root="train", train=True, download=True, transform=ToTensor())
val_dataset = datasets.MNIST(root="val", train=False, download=True, transform=ToTensor())
train_loader = DataLoader(train_dataset, batch_size=64, shuffle=True)
val_loader = DataLoader(val_dataset, batch_size=64, shuffle=True)

model = LeNet()
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
model = model.to(device)

learning_rate = 0.01
epochs = 10
criterion = nn.CrossEntropyLoss()
optimizer = optim.SGD(model.parameters(), lr=learning_rate)

def train():
    for i in range(epochs):
        acc = []
        losses = []
        model.train()
        for data in train_loader:
            input, target = data
            optimizer.zero_grad()
            output = model(input)
            loss = criterion(output, target)
            loss.backward()
            optimizer.step()

            with torch.no_grad():
                predicted = torch.argmax(output, dim=1)
                acc.append((predicted == target).sum().item()/len(target))
                losses.append(loss)
        print(f"{i}. train, acc: {sum(acc)/len(acc)}, loss: {sum(losses)/len(losses)}")

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
        torch.save(model, 'model.pt')
        torch.save(model.state_dict(), 'model_weights.pt')

if __name__ == '__main__':
    summary(model, (1,28,28))
    train()
