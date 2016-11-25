s.boot;
OSCdef(\tester, {
});

NetAddr.langPort
OSCFunc.trace(true);
OSCFunc.trace(false);

(
OSCdef(\block, { | ...args |
	args.postln;
}, '/block/touch/on');
)

