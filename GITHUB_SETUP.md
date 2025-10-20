# GitHub Actions Setup & Troubleshooting

## ✅ Step 1: Configure Your PAT (Personal Access Token)

Your PAT has been configured to use macOS Keychain. Next time you use git commands in terminal:

```bash
# Try a simple command to trigger PAT prompt
git pull
```

When prompted:
- **Username**: `m9dful1` (your GitHub username)
- **Password**: Paste your PAT token here (NOT your GitHub password)

The PAT will be saved in your macOS Keychain and you won't need to enter it again.

## 🔍 Step 2: Check if GitHub Actions is Enabled

1. Go to your repository: https://github.com/m9dful1/Craftium
2. Click on the **"Actions"** tab at the top
3. Check if you see:
   - ✅ **If you see workflow runs**: Actions is enabled! Check the run status
   - ❌ **If you see "Get started with GitHub Actions"**: Actions needs to be enabled

### To Enable GitHub Actions:
1. Go to repository **Settings** tab
2. Click **Actions** → **General** in the left sidebar
3. Under "Actions permissions":
   - Select **"Allow all actions and reusable workflows"**
4. Under "Workflow permissions":
   - Select **"Read and write permissions"** ✅
   - Check **"Allow GitHub Actions to create and approve pull requests"** ✅
5. Click **Save**

## 🚀 Step 3: Check Workflow Status

### View Recent Workflow Runs:
Visit: https://github.com/m9dful1/Craftium/actions

You should see:
- **Continuous Build** workflow (triggered by your recent push)
- Status: ⏳ Running, ✅ Success, or ❌ Failed

### If No Workflows Appear:

#### Option A: Trigger a New Build (Make a Small Change)

```bash
cd /Users/cthul/Documents/Projects/Craftium

# Make a small change to trigger the workflow
echo "" >> README.md

# Commit and push
git add README.md
git commit -m "Trigger CI/CD workflow"
git push origin main
```

#### Option B: Manually Trigger Workflow

1. Go to: https://github.com/m9dful1/Craftium/actions
2. Click on **"Continuous Build"** workflow
3. Click **"Run workflow"** button
4. Select **main** branch
5. Click **"Run workflow"**

## 📦 Step 4: Check Build Output

Once the workflow runs (takes ~5-10 minutes):

### Option 1: Download from Workflow Artifacts
1. Go to: https://github.com/m9dful1/Craftium/actions
2. Click on the latest workflow run
3. Scroll to **"Artifacts"** section at the bottom
4. Download the DMG file

### Option 2: Download from Continuous Release
1. Go to: https://github.com/m9dful1/Craftium/releases/tag/continuous
2. Download `Craftium-macOS-*.dmg` under Assets

## 🔧 Troubleshooting

### Build Fails with Qt Error
The workflow installs Qt automatically, but if it fails:
- Check the logs in Actions tab
- The Qt version 6.7.3 might not be available - we can update the workflow

### Build Fails with CMake Error
- Check if CMakeLists.txt has the correct syntax
- The workflow uses CMake 3.16+ which should be available on GitHub runners

### Permission Denied Errors
- Ensure "Read and write permissions" is enabled in repository settings
- The `GITHUB_TOKEN` secret is automatically provided by GitHub

### Workflow Not Triggering
- Make sure you pushed to `main` or `master` branch
- Check if Actions is enabled in repository settings
- Try making a new commit and pushing again

## 📱 Quick Test

Run this to verify your setup and trigger a build:

```bash
cd /Users/cthul/Documents/Projects/Craftium

# Update README with badge (if not already there)
echo "" >> README.md
echo "Last updated: $(date)" >> README.md

# Commit and push
git add README.md
git commit -m "Test CI/CD workflow trigger"
git push origin main
```

Then immediately check: https://github.com/m9dful1/Craftium/actions

You should see a new workflow run appear within a few seconds!

## ✨ Expected Results

After a successful workflow run, you'll have:

1. **Workflow Artifacts** (available for 90 days)
   - `Craftium-macOS-[date]-[sha].dmg`
   - Raw `Craftium` executable

2. **Continuous Release** (always latest)
   - Tag: `continuous`
   - Pre-release badge
   - DMG file ready for download
   - Installation instructions

3. **Build Logs** (for debugging)
   - Full compilation output
   - Qt installation logs
   - DMG creation logs

## 🎯 Next Steps

Once your first build succeeds:

1. **Update README.md** - Replace `YOUR_USERNAME` with `m9dful1`
2. **Create v1.0.0 Release** - Tag your first official release
3. **Test the DMG** - Download and test the built application

---

**Need help?** Check the workflow logs in the Actions tab for detailed error messages.
