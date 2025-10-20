# Release Strategy Guide

## 📦 Two Types of Releases

### 1. Continuous Release (Automatic) ✅

**What**: Development builds that update automatically
**When**: Every push to the main branch
**Tag**: `continuous`
**Status**: Pre-release
**Link**: https://github.com/m9dful1/Craftium/releases/tag/continuous

**You already have this!** Nothing else needed for development.

#### Features:
- ✅ Auto-updates with every code change
- ✅ Always has the latest features and fixes
- ✅ Perfect for testing and development
- ✅ Includes installation instructions
- ✅ DMG file ready to download
- ⚠️ Marked as "Pre-release" (not for production use)

#### When to Use:
- Daily development work
- Testing new features
- Sharing builds with beta testers
- Quick iterations

---

### 2. Versioned Releases (Manual) 🏷️

**What**: Official stable releases with version numbers
**When**: When you're ready to release a stable version
**Tags**: `v1.0.0`, `v1.1.0`, `v2.0.0`, etc.
**Status**: Official release
**Create**: Manually, when ready

#### Example: Creating v1.0.0

**When you're ready** (e.g., after testing, bug fixes, etc.):

```bash
# Step 1: Create and push a tag
git tag -a v1.0.0 -m "Craftium v1.0.0 - Initial stable release"
git push origin v1.0.0

# Step 2: Go to GitHub and create the release
# https://github.com/m9dful1/Craftium/releases/new
```

Then on GitHub:
1. Select the `v1.0.0` tag
2. Set release title: "Craftium v1.0.0 - Auto Crafter for macOS"
3. Write release notes:

```markdown
## 🎉 Craftium v1.0.0 - Initial Release

The first official release of Craftium, a powerful keyboard automation tool for macOS!

### Features
- ⌨️ Record and replay keyboard sequences with precise timing
- 🔄 Repeat sequences up to 100 times
- 💾 Save and load sequences as JSON files
- 🎨 Dark mode support
- 📌 Enhanced "Always on Top" with native macOS window levels
- 🚀 Perfect for automating productivity tasks, software testing, and data entry

### Installation
1. Download `Craftium-macOS-v1.0.0.dmg` below
2. Open the DMG and drag Craftium.app to Applications
3. First run: Right-click → Open to bypass Gatekeeper
4. Grant Accessibility permissions in System Settings

### System Requirements
- macOS 11.0 (Big Sur) or later
- Apple Silicon or Intel Mac

### Known Issues
- First launch may take a few seconds
- Some fullscreen applications may override window levels

### Credits
Designed by Jeremy Matthews
© SpiritWise Studios LLC
```

4. Click "Publish release"
5. **GitHub Actions will automatically build and attach the DMG!**

#### Features:
- 🏷️ Semantic versioning (v1.0.0, v1.1.0, v2.0.0)
- 📝 Detailed release notes and changelog
- ✅ Marked as stable/production-ready
- 🎯 Users can download specific versions
- 📊 Tracks downloads per version

#### When to Use:
- First public release
- Major feature additions
- Important bug fixes
- Marketing/announcements

---

## 🗓️ Recommended Timeline

### Now (Day 1)
✅ **Use Continuous Releases**
- You have automatic builds working
- Download and test the DMG
- Make sure everything works correctly
- Test the "Always on Top" feature with your target applications

### Soon (Days 2-7)
🧪 **Testing Phase**
- Use the continuous builds for testing
- Fix any bugs you find
- Make improvements
- Each push creates a new continuous build automatically

### When Ready (Week 2+)
🚀 **Create v1.0.0**
- Once you're confident everything works
- No major bugs
- Features are complete
- Ready to share publicly

---

## 📊 Comparison Chart

| Feature | Continuous | Versioned (v1.0.0) |
|---------|------------|-------------------|
| Auto-updates | ✅ Every push | ❌ Manual only |
| Stable | ⚠️ Development | ✅ Stable |
| For testing | ✅ Yes | ❌ No |
| Public release | ❌ No | ✅ Yes |
| Version tracking | ❌ No | ✅ Yes |
| Auto-build | ✅ Yes | ✅ Yes |
| DMG included | ✅ Yes | ✅ Yes |

---

## 🎯 Quick Answer

**Q: Do I need to create a release manually?**

**A: NO!** for development work.

- ✅ The `continuous` release is **automatic**
- ✅ It updates **every time you push**
- ✅ Your DMG is **already available**

**A: YES!** when you want an official v1.0.0 release.

- But that's **optional** and **only when you're ready**
- Use the continuous release for now!

---

## 📱 What to Do Right Now

1. ✅ Download the DMG from the continuous release
2. ✅ Test it to make sure it works
3. ✅ Test the "Always on Top" feature with your applications
4. ✅ Continue developing and testing
5. ⏸️ Create v1.0.0 when you're ready for public release

---

**Bottom Line**: You're all set! No manual action needed for now. The continuous release has you covered. 🎉
