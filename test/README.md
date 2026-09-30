# ARM64 GPU validation

Run the host vector and version-1 file-layout tests from the repository root:

```sh
clang++ -std=c++17 -O2 test/vint.cpp -o /tmp/genefer-vint-test
/tmp/genefer-vint-test
clang++ -std=c++17 -O2 test/b_vec.cpp -o /tmp/genefer-layout-test
/tmp/genefer-layout-test
```

`gfn15-bases.txt` contains a known GFN15 prime (base 15547296), five known
composites whose bases are cubes, and two additional reference cases. The prime
is listed in [the Genefer presentation](https://ibethune.github.io/files/PPAM_Genefer.pdf).

Run in a new working directory, using absolute paths to the executable and fixture:

```sh
/path/to/geneferv -n15 -l8 -b /path/to/test/gfn15-bases.txt -f case -p
/path/to/geneferv -n15 -l8 -b /path/to/test/gfn15-bases.txt -f case -s
/path/to/geneferv -n15 -l8 -b /path/to/test/gfn15-bases.txt -f case -c
```

Check the success messages and `results.txt`, not only the process exit status.
The prime's proof key should be `0E8B100FCE895414`. All other candidates should
be composite. Proof keys must agree between proof generation and server mode;
certificate keys must agree between server mode and certificate verification.
Server mode randomizes its challenge, so independently generated certificate
keys need not agree.

For file compatibility, copy the ARM-generated `case.proof` into a new directory
on x86 and run server mode followed by certificate verification with an unmodified
x86 executable and `-cpu`. Also copy an x86-generated certificate to ARM and
verify it with the GPU build. Use the same bases, exponent and batch width.
Comparing mathematical keys alone does not test serialized file compatibility.

ARM64 CPU mode is intentionally unsupported. A valid command with `-cpu` must
fail with a clear error. For checkpoint validation, interrupt a proof computation
with SIGINT while it is running, confirm `case.chk` exists, then rerun the same
command. It must report resuming and produce the same result/proof keys as an
uninterrupted reference run.
