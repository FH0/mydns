# mydns

json configured dns server

### in

#### tag

The character string used for differentiation. If this item is empty or does not exist, it can only be matched if it is the first one and routing out_tag is empty or does not exist.

#### address

Listen address. Default: 0.0.0.0

#### port

Listen port. Default: 1053

### out

#### tag

The character string used for differentiation. If this item is empty or does not exist, it can only be matched globally.

#### address

The address of DNS server. Default: 8.8.8.8

#### port

The port of DNS server. Default: 53

### routing

#### in_tag

Match in tag. Default match the global.

#### domain

Use regular expressions to match domains. Default match the global.

#### out_tag

Match out tag. Default match the first.

### What can be improved

- per connect per endpoint
- timer
- tcp
