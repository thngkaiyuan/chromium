# Chromium

Chromium, but more secure. It is even better when paired with [this extension](https://github.com/thngkaiyuan/secure-channel).

When the channel is secure:
![image](https://cloud.githubusercontent.com/assets/10496851/23801645/db71b7ee-05eb-11e7-9f59-50de91658526.png)

When a man-in-the-middle tampers with the channel:
![image](https://cloud.githubusercontent.com/assets/10496851/23801924/b4b23b0a-05ec-11e7-85c5-5342646a3015.png)

# Security Properties

- Client to Origin Server Confidentiality & Integrity
- Origin Server to Client Confidentiality, Integrity & Authenticity

# Todo

- [x] Add logic and visuals for failing checks
- [x] Change names from "Integrity Guard" to "Secure Channel"
- [ ] Retrieve key and enrollment information from public key server
- [ ] Cache retrieved public keys
- [x] Add encryption for request body
- [x] Add AEAD for response body
- [x] Check integrity of request (e.g. prepend order number + hash of request header + body inside plaintext & check on server side)
