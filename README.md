# Chromium

Chromium, but more secure. It is even better when paired with [this extension](https://github.com/thngkaiyuan/secure-channel).

# Todo

- [x] Add logic and visuals for failing checks
- [x] Change names from "Integrity Guard" to "Secure Channel"
- [ ] Retrieve key and enrollment information from public key server
- [ ] Cache retrieved public keys
- [x] Add encryption for request body
- [x] Add AEAD for response body
- [ ] Check integrity of request (e.g. append hash of request header + body inside plaintext & check on server side)
