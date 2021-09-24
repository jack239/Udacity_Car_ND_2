FROM ubuntu:16.04
#install utils for build
RUN apt-get update \
  && apt-get install -y ssh \
    gcc \
    g++ \
    python \
    curl \
    rsync \
    libpcl-dev \
    libopenni2-dev \
    libusb-dev \
    apt-transport-https \
    wget \
    git \
  && apt-get clean

RUN cd /tmp \ 
    && wget https://cmake.org/files/v3.10/cmake-3.10.3.tar.gz \
    && tar xvf cmake-3.10.3.tar.gz \ 
    && cd cmake-3.10.3 \
    && ./bootstrap \
    && make \
    && make install \
    && cd .. \
    && rm -rf cmake*

RUN ln -s /usr/bin/vtk6 /usr/bin/vtk
RUN ln -s /usr/lib/python2.7/dist-packages/vtk/libvtkRenderingPythonTkWidgets.x86_64-linux-gnu.so /usr/lib/x86_64-linux-gnu/libvtkRenderingPythonTkWidgets.so

RUN apt-get update \
  && apt-get install -y ssh \
    libuv1-dev \ 
    libssl-dev \ 
    libz-dev \
    libgoogle-glog-dev \
    libgtest-dev \
    liblapack-dev \
  && apt-get clean

#build google tests
RUN cd /usr/src/gtest \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make \
  &&  cp *.a /usr/lib \
  && cd .. \
  && rm -rf build

#build uWebSockets
RUN cd /tmp \
  && git clone https://github.com/uWebSockets/uWebSockets \
  && cd uWebSockets \
  && git checkout e94b6e1 \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make \
  && make install \
  && cd /tmp \
  && rm -rf  uWebSockets \
  && ln -s /usr/lib64/libuWS.so /usr/lib/libuWS.so




#set enviroment for remotr build
RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \
  ) > /etc/ssh/sshd_config_test_clion \
  && mkdir /run/sshd

RUN useradd -m user \
  && yes password | passwd user

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]
