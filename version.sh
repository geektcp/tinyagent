sed -i "/CREATETIME/{s/.*/#define CREATETIME      \"`date +%Y-%m-%d--%H:%M:%S`\"/}" ./src/include/common.h
