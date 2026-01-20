"""
Pytest configuration and fixtures
This file ensures proper Python path for imports
"""
import sys
from pathlib import Path

# Add tests directory to Python path so tests can import config.py
tests_dir = Path(__file__).parent
sys.path.insert(0, str(tests_dir))
