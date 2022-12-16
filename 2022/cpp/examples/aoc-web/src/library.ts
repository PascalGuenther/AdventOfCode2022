type WasmCallbacks = import("./WasmCallbacks").WasmCallbacks;

declare function mergeInto(...args: any[]): any;
declare var LibraryManager: any;
declare function UTF8ToString(ptr: number, len: number): string;

declare var aocWasmCallbacks: WasmCallbacks;

mergeInto(LibraryManager.library, {
  jsReceivePuzzleSolutionCb: function (
    day: number,
    part: number,
    output: number,
    outputLength: number
  ) {
    let solution =
      output !== 0 && outputLength > 0
        ? UTF8ToString(output, outputLength)
        : undefined;
    aocWasmCallbacks.reiceivePuzzleSolution(day, part, solution);
  },
  jsDebugCb: function (code: number) {
    aocWasmCallbacks.debug?.(code);
  },
});
