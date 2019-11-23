FROM ubuntu:bionic

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get -y install \
    software-properties-common \
    curl \
    python3-minimal \
    python3-dev \
    inotify-tools \
    && \
    apt-get clean 

RUN curl https://bootstrap.pypa.io/get-pip.py -o /tmp/get-pip.py && \
    python3 /tmp/get-pip.py --prefix=/usr/local/ && \
    rm /tmp/get-pip.py

CMD bash -x

RUN mkdir -p /opt/app

WORKDIR /opt/app

COPY requires .
RUN pip install -r requires
