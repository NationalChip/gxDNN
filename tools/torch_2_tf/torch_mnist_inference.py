from __future__ import print_function
import argparse
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision import datasets, transforms
from torch.optim.lr_scheduler import StepLR
import numpy as np


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.conv1 = nn.Conv2d(1, 32, 5, 1)
        self.conv2 = nn.Conv2d(32, 32, 5, 1)
        #self.dropout1 = nn.Dropout(0.25)
        self.fc1 = nn.Linear(4*4*32, 10)

    def forward(self, x):
        x = self.conv1(x)
        x = F.relu(x)
        x = F.max_pool2d(x, 2)
        x = self.conv2(x)
        x = F.relu(x)
        x = F.max_pool2d(x, 2)
        #x = self.dropout1(x)
        fc1_h, fc1_w, fc1_c = x.shape[2], x.shape[3], x.shape[1]
        x = torch.flatten(x, 1)
        x = self.fc1(x)
        x = F.relu(x)
        output = F.log_softmax(x, dim=1)
        return output

def test(model, device, test_loader):
    model.eval()
    test_loss = 0
    correct = 0
    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
            #print(data[0])
            #print(target[0])
            #print(data[1])
            #print(target[1])
            output = model(data)
            test_loss += F.nll_loss(output, target, reduction='sum').item()  # sum up batch loss
            pred = output.argmax(dim=1, keepdim=True)  # get the index of the max log-probability
            correct += pred.eq(target.view_as(pred)).sum().item()

    test_loss /= len(test_loader.dataset)

    print('\nTest set: Average loss: {:.4f}, Accuracy: {}/{} ({:.0f}%)\n'.format(
        test_loss, correct, len(test_loader.dataset),
        100. * correct / len(test_loader.dataset)))

def save_tf_weights(npz_file, state_dict):
    new_state_dict = {}
    for k, v in state_dict.items():
        if 'weight' in k:
            if len(v.shape) == 4: # conv
                # [co,ci,h,w] --> [h,w,ci,co]
                v = v.permute(2, 3, 1, 0)
            elif k == "fc1.weight": # fc
                # pytorch中卷积格式是chw，tf中是hwc，这里fc的输入channel是从卷积的输出chw
                # reshape而来，对应在tf中输入channel需要转置成hwc
                co, ci = v.shape[0], v.shape[1]
                v = v.reshape(co, 32, 4, 4)
                v = v.permute(0, 2, 3, 1) # [c,h,w] --> [h,w,c]
                v = v.reshape(co, ci)
                v = v.permute(1, 0) # [co,ci] --> [ci,co]
        new_state_dict[k] = v
    np.savez(npz_file, **new_state_dict)

def main():
    # Training settings
    parser = argparse.ArgumentParser(description='PyTorch MNIST Example')
    parser.add_argument('--batch-size', type=int, default=64, metavar='N',
                        help='input batch size for training (default: 64)')
    parser.add_argument('--test-batch-size', type=int, default=1000, metavar='N',
                        help='input batch size for testing (default: 1000)')
    parser.add_argument('--epochs', type=int, default=2, metavar='N',
                        help='number of epochs to train (default: 2)')
    parser.add_argument('--lr', type=float, default=1.0, metavar='LR',
                        help='learning rate (default: 1.0)')
    parser.add_argument('--gamma', type=float, default=0.7, metavar='M',
                        help='Learning rate step gamma (default: 0.7)')
    parser.add_argument('--no-cuda', action='store_true', default=False,
                        help='disables CUDA training')
    parser.add_argument('--dry-run', action='store_true', default=False,
                        help='quickly check a single pass')
    parser.add_argument('--seed', type=int, default=1, metavar='S',
                        help='random seed (default: 1)')
    parser.add_argument('--log-interval', type=int, default=10, metavar='N',
                        help='how many batches to wait before logging training status')
    parser.add_argument('--save-model', action='store_true', default=True,
                        help='For Saving the current Model')
    args = parser.parse_args()
    use_cuda = not args.no_cuda and torch.cuda.is_available()

    torch.manual_seed(args.seed)

    device = torch.device("cuda" if use_cuda else "cpu")

    test_kwargs = {'batch_size': args.test_batch_size}
    if use_cuda:
        cuda_kwargs = {'num_workers': 1,
                       'pin_memory': True,
                       'shuffle': True}
        test_kwargs.update(cuda_kwargs)

    transform=transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.1307,), (0.3081,))
        ])
    dataset2 = datasets.MNIST('../data', train=False,
                       transform=transform)
    test_loader = torch.utils.data.DataLoader(dataset2, **test_kwargs)

    model = Net().to(device)
    print('===> load pretrained model')
    state_dict = torch.load('mnist_cnn.pt', map_location=lambda storage, loc: storage)
    save_tf_weights('model_weights.npz', state_dict)
    model.load_state_dict(state_dict)
    test(model, device, test_loader)


if __name__ == '__main__':
    main()
