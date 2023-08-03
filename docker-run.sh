#!/bin/sh

docker run -v $(pwd):/opt/chplx -it --rm chplx-dev /bin/bash
