#!/usr/bin/env python3
"""
HTTP Server Stress Test Script
Tests various endpoints with concurrent connections to measure:
- Requests per second (RPS)
- Latency (min/avg/max)
- Success rate
- Error rate
"""

import argparse
import concurrent.futures
import statistics
import time
import urllib.request
import urllib.error
import threading
from dataclasses import dataclass
from typing import List, Tuple

@dataclass
class RequestResult:
    success: bool
    status_code: int
    latency_ms: float
    error: str = ""
    response_size: int = 0

class StressTest:
    def __init__(self, host: str, port: int):
        self.base_url = f"http://{host}:{port}"
        self.results: List[RequestResult] = []
        self.lock = threading.Lock()

    def make_request(self, endpoint: str, method: str = "GET", data: bytes = None) -> RequestResult:
        url = f"{self.base_url}{endpoint}"
        start_time = time.time()

        try:
            req = urllib.request.Request(url, data=data, method=method)
            req.add_header('Connection', 'keep-alive')

            with urllib.request.urlopen(req, timeout=10) as response:
                body = response.read()
                latency = (time.time() - start_time) * 1000

                return RequestResult(
                    success=True,
                    status_code=response.status,
                    latency_ms=latency,
                    response_size=len(body)
                )
        except urllib.error.HTTPError as e:
            latency = (time.time() - start_time) * 1000
            return RequestResult(
                success=False,
                status_code=e.code,
                latency_ms=latency,
                error=str(e)
            )
        except Exception as e:
            latency = (time.time() - start_time) * 1000
            return RequestResult(
                success=False,
                status_code=0,
                latency_ms=latency,
                error=str(e)
            )

    def run_concurrent_test(self, endpoint: str, num_requests: int,
                           concurrency: int, method: str = "GET",
                           data: bytes = None) -> List[RequestResult]:
        results = []

        def worker():
            result = self.make_request(endpoint, method, data)
            with self.lock:
                results.append(result)

        with concurrent.futures.ThreadPoolExecutor(max_workers=concurrency) as executor:
            futures = [executor.submit(worker) for _ in range(num_requests)]
            concurrent.futures.wait(futures)

        return results

def print_results(results: List[RequestResult], test_name: str, concurrency: int):
    successful = [r for r in results if r.success]
    failed = [r for r in results if not r.success]

    if not results:
        print(f"\n{test_name}: No results")
        return

    latencies = [r.latency_ms for r in results]

    print(f"\n{'='*60}")
    print(f"Test: {test_name}")
    print(f"{'='*60}")
    print(f"Total Requests:     {len(results)}")
    print(f"Successful:         {len(successful)} ({len(successful)/len(results)*100:.1f}%)")
    print(f"Failed:             {len(failed)} ({len(failed)/len(results)*100:.1f}%)")

    if latencies:
        print(f"\nLatency Statistics:")
        print(f"  Min:              {min(latencies):.2f} ms")
        print(f"  Avg:              {statistics.mean(latencies):.2f} ms")
        print(f"  Max:              {max(latencies):.2f} ms")
        print(f"  Median:           {statistics.median(latencies):.2f} ms")
        if len(latencies) > 1:
            print(f"  Std Dev:          {statistics.stdev(latencies):.2f} ms")

    if latencies:
        theoretical_rps = 1000 / statistics.mean(latencies) if latencies else 0
        print(f"\nThroughput:")
        print(f"  Est. RPS per conn:  ~{theoretical_rps:.0f} req/s")
        print(f"  Est. Total RPS:    ~{theoretical_rps * concurrency:.0f} req/s")

    if failed:
        error_types = {}
        for f in failed:
            error_key = f.error if f.error else f"HTTP {f.status_code}"
            error_types[error_key] = error_types.get(error_key, 0) + 1
        print(f"\nError Breakdown:")
        for error, count in sorted(error_types.items(), key=lambda x: -x[1]):
            print(f"  {error}: {count}")

def check_server_health(test: StressTest) -> bool:
    """Check if server is running before tests"""
    print("Checking server health...")
    try:
        result = test.make_request("/health")
        if result.success:
            print(f"Server is healthy! Status: {result.status_code}")
            return True
        else:
            print(f"Server returned error: {result.status_code}")
            return False
    except Exception as e:
        print(f"Cannot connect to server: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description='HTTP Server Stress Test')
    parser.add_argument('--host', default='localhost', help='Server host')
    parser.add_argument('--port', type=int, default=12345, help='Server port')
    parser.add_argument('--requests', type=int, default=1000, help='Total requests per test')
    parser.add_argument('--concurrency', type=int, default=10, help='Concurrent connections')
    parser.add_argument('--quick', action='store_true', help='Quick test with fewer requests')

    args = parser.parse_args()

    if args.quick:
        args.requests = 100
        args.concurrency = 5

    test = StressTest(args.host, args.port)

    # Health check
    if not check_server_health(test):
        print("\nError: Server is not responding. Please start the server first.")
        print(f"Expected server at: http://{args.host}:{args.port}")
        return 1

    print(f"\nStarting stress test with {args.requests} requests, {args.concurrency} concurrent connections")
    print("="*60)

    # Test 1: Simple GET requests
    print(f"\n[1/6] Testing simple GET requests to /api/json...")
    results = test.run_concurrent_test("/api/json", args.requests, args.concurrency)
    print_results(results, "Simple GET /api/json", args.concurrency)

    # Test 2: Health check endpoint
    print(f"\n[2/6] Testing health check endpoint...")
    results = test.run_concurrent_test("/health", args.requests, args.concurrency)
    print_results(results, "GET /health", args.concurrency)

    # Test 3: Large response
    print(f"\n[3/6] Testing large response endpoint...")
    results = test.run_concurrent_test("/api/large", args.requests // 2, args.concurrency)
    print_results(results, "GET /api/large (~10KB)", args.concurrency)

    # Test 4: POST with body
    print(f"\n[4/6] Testing POST requests with body...")
    post_data = b'{"test":"data","count":123}'
    results = test.run_concurrent_test("/api/echo", args.requests // 2, args.concurrency, "POST", post_data)
    print_results(results, "POST /api/echo", args.concurrency)

    # Test 5: Delayed responses (simulates processing time)
    print(f"\n[5/6] Testing delayed responses (10ms delay)...")
    results = test.run_concurrent_test("/api/delay/10", args.requests // 4, args.concurrency)
    print_results(results, "GET /api/delay/10", args.concurrency)

    # Test 6: 404 responses
    print(f"\n[6/6] Testing 404 responses...")
    results = test.run_concurrent_test("/nonexistent/path", args.requests // 4, args.concurrency)
    print_results(results, "GET /nonexistent (404)", args.concurrency)

    print(f"\n{'='*60}")
    print("Stress test completed!")
    print(f"{'='*60}")

    return 0

if __name__ == "__main__":
    exit(main())
