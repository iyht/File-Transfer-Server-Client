import numpy as np
import cv2
import os
from scipy import linalg
import matplotlib.pyplot as plt
import matplotlib
import dlib
import sys
matplotlib.use('TkAgg')
person1 = 'Haotian'
person2 = 'Gaojing'
path = './datasets/'

def plot_face(images, h, w, n_row, n_col, single=False):
    plt.figure(figsize=(2.2 * n_col, 2.2 * n_row))
    plt.subplots_adjust(bottom=0, left=.01, right=.99, top=.90, hspace=.20)
    for i in range(n_row * n_col):
        plt.subplot(n_row, n_col, i + 1)
        if single:  plt.imshow(images.reshape((h, w)), cmap=plt.cm.gray)
        else: plt.imshow(images[i].reshape((h, w)), cmap=plt.cm.gray)
        plt.xticks(())
        plt.yticks(())
    plt.show()
def read_img(path, numbers, img_lst):
    i = 0
    for image_filename in os.listdir(path):
        if 'png' in image_filename or 'jpg' in image_filename: 
            fullpath = path+'/'+image_filename
            img = cv2.imread(fullpath, 0)
            # reshape to MNx1
            img = np.ravel(img)
            img_lst.append(img)
            i += 1
            if(i == numbers):
                break
def who(test_img):
    test_img = test_img.reshape(-1, 1) # 10000 x 1
    test_weights = np.dot(V, test_img-avgface_all) # K x 1. means how each eigenface contribute to build test_img
    print('test_img:', test_img.shape, 'avgface:', avgface_all.shape, 'test-avg',(test_img-avgface_all).shape )

    # compare with weights in the training set.
    # TODO: need find way to detect unknown faces
    min_distance, index_img= np.inf, -1
    counter = 0
    for column in weights.T:
        column = column.reshape(-1, 1)
        #distance = np.sqrt(sum(i**2 for i in (column - test_weights)))
        #print('column:', column.reshape(-1,1).shape, 'test_weight', test_weights.shape)
        distance = np.linalg.norm(column - test_weights)
        if distance < min_distance:
            min_distance, index_img = distance, counter
        counter += 1
    print(min_distance, index_img) 
    if index_img >= read_num: return(person2)
    else: return (person1)

# 1. read training img(MxN) and reshape to img_vector(MNx1)
img_lst = []
read_num = 200
read_img(path+person1, read_num, img_lst)
read_img(path+person2, read_num, img_lst)

# 2. put all img_vector into Matrix and compute the avg_face vactor
img_array = np.array(img_lst) # read_num x 10000
img_array = img_array.T # 10000 x read_num
avgface_person1 = np.mean(img_array[:,:read_num], axis=1).reshape(-1, 1)
avgface_person2 = np.mean(img_array[:, read_num:], axis=1).reshape(-1,1)
avgface_all = img_array.mean(axis=1, keepdims=True)#np.mean(img_array, axis=1)
#print('avgface_person1', avgface_person1.shape)

# 3. substract all vector in Matrix with avg_face
C = img_array-avgface_all # 10000 x read_num

# 4. compute covariance matrix C*C.T(10000x10000), this is expensive. We need calculate C.T*C(read_num x read_num) first
CTC = np.dot(C.T, C) / (read_num)# read_num x read_num

# 5. calculate the eigenvectors and eigenvalue of covariance matrix. We will keep Kth conponent
# TODO: find the best K
K = 100

# two ways to get eigenfaces
# 5.1 svd of C
U, s, V = np.linalg.svd(C.T, full_matrices=False)# U(read_num x read_num)
V = V[:K] # K x 10000
# for visualization, we need to normaliza eigenvector when using SVD
#for i in range(V.shape[0]):
#    #print(i)
#    V[i] = 255*(V[i]-min(V[i]))/(max(V[i])-min(V[i]))


# 5.2 eig of CTC
#w, v = np.linalg.eig(CTC)
## C*(eigen vector if CTC)
#U_CCT = np.dot(C, U).T
#U_CCT = U_CCT[:K] # K x 10000
# w is eigenvalue, v is eigenvector. U equal v if use svd


# 6. (reconstruct data)project avg train img of each person onto the Eigenface space. Remenber that the job of PCA is to reduce redundant dimension of data
weights = np.dot(V, C) # K x read_num

#avgfaces = np.concatenate((avgface_person1, avgface_person2),axis = 1)
#print('avgfaces:', avgfaces.shape)
#weights_meanfaces = np.dot(V, avgfaces)
#print('weights_meanfaces', weights_meanfaces.shape)
recon_img = np.dot(weights[:,0].T, V).reshape(-1, 1) + avgface_all
plot_face([avgface_person1, avgface_all, recon_img, img_array[:,0]], 100,100,1,4, )
#eigenface1 = np.matmul(U_CCT.transpose(), C[:20w])
#eigenface2 = np.matmul(U_CCT.transpose(), C[read_num:read_num+20])
#print('U_CCT',U_CCT)
#print(U_CCT.shape)
#eiface= 255*(V[0]-min(V[0]))/(max(V[0])-min(V[0]))
#plot_face(U_CCT, 100,100,4,4)
#plot_face(V,100,100,4,4)
#plot_face(projections, 100, 100, 4,4)

# 7. Testing: we take the test img project onto the Eigenface space. Find the minimum Euclidean distance among train data on the Eigenface space where we got in step 6.

detector = dlib.get_frontal_face_detector()
print(sys.argv)
if len(sys.argv)==1:
    cam = cv2.VideoCapture(-1)  
    while True:  
        _, img = cam.read()  
        gray_image = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        dets = detector(gray_image, 1)
        if not len(dets):
            #print('Can`t get face.')
            cv2.imshow('img', img)
            key = cv2.waitKey(30) & 0xff  
            if key == 27:
                sys.exit(0)
                
        for i, d in enumerate(dets):
            x1 = d.top() if d.top() > 0 else 0
            y1 = d.bottom() if d.bottom() > 0 else 0
            x2 = d.left() if d.left() > 0 else 0
            y2 = d.right() if d.right() > 0 else 0
            face = gray_image[x1:y1,x2:y2]
            face = cv2.resize(face, (100,100))
            face = np.ravel(face)
            print(who(face))

            cv2.rectangle(img, (x2,x1),(y2,y1), (255,0,0),3)
            cv2.imshow('image',img)
            key = cv2.waitKey(30) & 0xff
            if key == 27:
                sys.exit(0)
else:
    img = cv2.imread(sys.argv[1], 0)
    print(img.shape)
    gray_image = cv2.resize(img, (100,100))
    dets = detector(gray_image, 1)
    if not len(dets):
        print('Can`t get face.')
        plot_face(img, 100,100, 1,1, single=True) 
    for i, d in enumerate(dets):
        x1 = d.top() if d.top() > 0 else 0
        y1 = d.bottom() if d.bottom() > 0 else 0
        x2 = d.left() if d.left() > 0 else 0
        y2 = d.right() if d.right() > 0 else 0
        face = gray_image[x1:y1,x2:y2]
        face = cv2.resize(face, (100,100))
        face = np.ravel(face)
        print('checking....')
        print(who(face))

